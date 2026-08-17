// Pigeon CLI — file runner and interactive REPL.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "pigeon.h"

// Forward declarations for stdlib functions (avoids pulling in internal VM headers)
const char*  pigeonStdlibLoadModule(const char* name);
const char*  pigeonStdlibGetDefaultExport(const char* name);
const char** pigeonStdlibGetExports(const char* name);
PigeonForeignMethodFn pigeonStdlibBindForeign(PigeonVM* vm, const char* module,
    const char* className, bool isStatic, const char* signature);
PigeonForeignClassMethods pigeonStdlibBindForeignClass(PigeonVM* vm,
    const char* module, const char* className);

#define MAX_LINE_LENGTH 4096
#define HISTORY_SIZE 100

// ANSI color codes for pretty output
#define COLOR_RESET   "\x1b[0m"
#define COLOR_PROMPT  "\x1b[36m"      // Cyan
#define COLOR_OUTPUT  "\x1b[32m"      // Green
#define COLOR_ERROR   "\x1b[31m"      // Red
#define COLOR_INFO    "\x1b[90m"      // Gray

// Command history
static char history[HISTORY_SIZE][MAX_LINE_LENGTH];
static int history_count = 0;
static int history_index = 0;

// Multi-line input buffer
static char input_buffer[MAX_LINE_LENGTH * 10];
static bool in_multiline = false;


static void writeFn(PigeonVM* vm, const char* text)
{
  printf("%s%s%s", COLOR_OUTPUT, text, COLOR_RESET);
}

static void errorFn(PigeonVM* vm, PigeonErrorType errorType,
                    const char* module, const int line,
                    const char* msg)
{
  switch (errorType)
  {
    case PIGEON_ERROR_COMPILE:
      fprintf(stderr, "%s[%s line %d] Error: %s%s\n",
              COLOR_ERROR, module, line, msg, COLOR_RESET);
      break;
    case PIGEON_ERROR_STACK_TRACE:
      fprintf(stderr, "%s[%s line %d] in %s%s\n",
              COLOR_ERROR, module, line, msg, COLOR_RESET);
      break;
    case PIGEON_ERROR_RUNTIME:
      fprintf(stderr, "%s[Runtime Error] %s%s\n",
              COLOR_ERROR, msg, COLOR_RESET);
      break;
  }
}

static void printBanner(void)
{
  printf("%s", COLOR_INFO);
  printf("╔═══════════════════════════════════════════════════════════╗\n");
  printf("║                   Pigeon REPL v%s                      ║\n",
         PIGEON_VERSION_STRING);
  printf("║          Interactive Read-Eval-Print Loop                 ║\n");
  printf("╚═══════════════════════════════════════════════════════════╝\n");
  printf("\n");
  printf("Commands:\n");
  printf("  .help            - Show this help message\n");
  printf("  .quit            - Exit the REPL\n");
  printf("  .clear           - Clear the screen\n");
  printf("  .vars            - List variables defined in the REPL session\n");
  printf("  .inspect <name>  - Show class, methods, and type info for <name>\n");
  printf("  .reset           - Reset the VM (clear all state)\n");
  printf("\n");
  printf("Tips:\n");
  printf("  - End expressions with no semicolon to print the result\n");
  printf("  - Use { } for multi-line blocks\n");
  printf("  - Press Ctrl+C to cancel current input\n");
  printf("%s\n", COLOR_RESET);
}

static void printHelp(void)
{
  printf("%s", COLOR_INFO);
  printf("\nPigeon REPL Commands:\n");
  printf("  .help            - Show this help message\n");
  printf("  .quit / .exit    - Exit the REPL\n");
  printf("  .clear / .cls    - Clear the screen\n");
  printf("  .vars            - List variables defined in the REPL session\n");
  printf("  .inspect <name>  - Show type, class, and methods for a variable or class\n");
  printf("  .reset           - Reset the VM and clear all state\n");
  printf("\nExamples:\n");
  printf("  pigeon> 2 + 2\n");
  printf("  pigeon> var x = 10\n");
  printf("  pigeon> System.print(\"Hello, World!\")\n");
  printf("  pigeon> [1, 2, 3].map {|n| n * 2 }\n");
  printf("  pigeon> \"\"\"                  // Start multi-line\n");
  printf("  ....> class Foo {\n");
  printf("  ....>   bar { 42 }\n");
  printf("  ....> }\n");
  printf("  ....> \"\"\"                    // End multi-line\n");
  printf("%s\n", COLOR_RESET);
}

static void addToHistory(const char* line)
{
  if (history_count < HISTORY_SIZE)
  {
    strncpy(history[history_count], line, MAX_LINE_LENGTH - 1);
    history[history_count][MAX_LINE_LENGTH - 1] = '\0';
    history_count++;
  }
  else
  {
    // Shift history and add to end
    for (int i = 0; i < HISTORY_SIZE - 1; i++)
    {
      strncpy(history[i], history[i + 1], MAX_LINE_LENGTH);
    }
    strncpy(history[HISTORY_SIZE - 1], line, MAX_LINE_LENGTH - 1);
    history[HISTORY_SIZE - 1][MAX_LINE_LENGTH - 1] = '\0';
  }
}

static bool isIncompleteBlock(const char* code)
{
  int braceCount = 0;
  int parenCount = 0;
  int bracketCount = 0;
  bool inString = false;
  bool inComment = false;

  for (const char* p = code; *p; p++)
  {
    if (inComment)
    {
      if (*p == '\n') inComment = false;
      continue;
    }

    if (*p == '/' && *(p + 1) == '/')
    {
      inComment = true;
      continue;
    }

    if (*p == '"' && (p == code || *(p - 1) != '\\'))
    {
      inString = !inString;
      continue;
    }

    if (inString) continue;

    switch (*p)
    {
      case '{': braceCount++; break;
      case '}': braceCount--; break;
      case '(': parenCount++; break;
      case ')': parenCount--; break;
      case '[': bracketCount++; break;
      case ']': bracketCount--; break;
    }
  }

  return braceCount > 0 || parenCount > 0 || bracketCount > 0;
}

static char* readLine(const char* prompt)
{
  static char line[MAX_LINE_LENGTH];

  printf("%s%s%s ", COLOR_PROMPT, prompt, COLOR_RESET);
  fflush(stdout);

  if (fgets(line, sizeof(line), stdin) == NULL)
  {
    return NULL;
  }

  // Remove trailing newline
  size_t len = strlen(line);
  if (len > 0 && line[len - 1] == '\n')
  {
    line[len - 1] = '\0';
  }

  return line;
}

static const char* replResolveModule(PigeonVM* vm, const char* importer,
                                     const char* module)
{
  (void)vm; (void)importer;
  return module;
}

static void replLoadModuleComplete(PigeonVM* vm, const char* module,
                                   PigeonLoadModuleResult result)
{
  (void)vm; (void)module;
  if (result.source) free((void*)result.source);
}

static PigeonLoadModuleResult replLoadModule(PigeonVM* vm, const char* module)
{
  PigeonLoadModuleResult result = {0};

  // Try stdlib first.
  const char* src = pigeonStdlibLoadModule(module);
  if (src != NULL) {
    result.source = src;
    return result;
  }

  // Fall back to file on disk (module name + ".wren").
  char path[4096];
  snprintf(path, sizeof(path), "%s.wren", module);
  FILE* f = fopen(path, "r");
  if (f) {
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return result; }
    fseek(f, 0, SEEK_SET);
    char* buf = malloc((size_t)sz + 1);
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { free(buf); fclose(f); return result; }
    buf[sz] = '\0';
    fclose(f);
    result.source = buf;
    result.onComplete = replLoadModuleComplete;
  }
  return result;
}

static PigeonForeignMethodFn replBindForeignMethod(PigeonVM* vm,
    const char* module, const char* className, bool isStatic,
    const char* signature)
{
  return pigeonStdlibBindForeign(vm, module, className, isStatic, signature);
}

static PigeonForeignClassMethods replBindForeignClass(PigeonVM* vm,
    const char* module, const char* className)
{
  return pigeonStdlibBindForeignClass(vm, module, className);
}

static const char* replResolveDefaultExport(PigeonVM* vm, const char* name)
{
  (void)vm;
  return pigeonStdlibGetDefaultExport(name);
}

static const char** replResolveExports(PigeonVM* vm, const char* name)
{
  (void)vm;
  return pigeonStdlibGetExports(name);
}

static void replInitConfig(PigeonConfiguration* config)
{
  pigeonInitConfiguration(config);
  config->writeFn                = writeFn;
  config->errorFn                = errorFn;
  config->resolveModuleFn        = replResolveModule;
  config->loadModuleFn           = replLoadModule;
  config->bindForeignMethodFn    = replBindForeignMethod;
  config->bindForeignClassFn     = replBindForeignClass;
  config->resolveDefaultExportFn = replResolveDefaultExport;
  config->resolveExportsFn       = replResolveExports;
}

// Each command invocation gets a unique module name so imports/vars don't collide.
static int g_cmdSeq = 0;

static void cmdVars(PigeonVM* vm)
{
  char mod[64];
  snprintf(mod, sizeof(mod), "repl.cmd%d", g_cmdSeq++);

  static const char* src =
    "import \"meta\" for meta\n"
    "var names = meta.getModuleVariables(\"repl\")\n"
    "if (names == null || names.count == 0) {\n"
    "  System.print(\"\x1b[90mNo variables defined.\x1b[0m\")\n"
    "} else {\n"
    "  for (name in names) {\n"
    "    System.print(\"\x1b[90m%(name)\x1b[0m\")\n"
    "  }\n"
    "}\n";
  pigeonInterpret(vm, mod, src);
}

// .inspect takes a Wren identifier (optionally dotted). Reject anything
// else so the token cannot be interpolated as arbitrary source.
static bool isSafeInspectName(const char* name)
{
  if (name == NULL || *name == '\0') return false;
  if (!(isalpha((unsigned char)*name) || *name == '_')) return false;
  for (const char* p = name + 1; *p != '\0'; p++)
  {
    if (!(isalnum((unsigned char)*p) || *p == '_' || *p == '.')) return false;
  }
  return true;
}

static void cmdInspect(PigeonVM* vm, const char* name)
{
  if (!isSafeInspectName(name))
  {
    printf("%s.inspect: name must be an identifier%s\n", COLOR_ERROR, COLOR_RESET);
    return;
  }

  // Wrap everything in a block so locals are block-scoped, not module-scoped.
  // This lets us call .inspect multiple times without re-declaration errors.
  // The block runs in "repl" so it can read user-defined variables.
  // We store map values in named locals before using them to avoid string
  // interpolation with quoted map keys (which breaks Wren's parser).
  char src[2048];
  snprintf(src, sizeof(src),
    "{\n"
    "  var info_ = System.inspect(%s)\n"
    "  var cn_   = info_[\"className\"]\n"
    "  var ic_   = info_[\"isClass\"]\n"
    "  var tag_  = ic_ ? \"class\" : \"instance of\"\n"
    "  System.print(\"\x1b[90m%s\x1b[0m : \x1b[36m\" + tag_ + \" \" + cn_ + \"\x1b[0m\")\n"
    "  var stat_ = []\n"
    "  var inst_ = []\n"
    "  for (e_ in info_[\"methods\"]) {\n"
    "    var s_ = e_.value[\"isStatic\"]\n"
    "    if (s_) {\n"
    "      stat_.add(e_.key)\n"
    "    } else {\n"
    "      inst_.add(e_.key)\n"
    "    }\n"
    "  }\n"
    "  stat_.sort()\n"
    "  inst_.sort()\n"
    "  if (stat_.count > 0) {\n"
    "    System.print(\"\x1b[90m  static:\x1b[0m\")\n"
    "    for (m_ in stat_) { System.print(\"    \x1b[32m\" + m_ + \"\x1b[0m\") }\n"
    "  }\n"
    "  if (inst_.count > 0) {\n"
    "    System.print(\"\x1b[90m  instance:\x1b[0m\")\n"
    "    for (m_ in inst_) { System.print(\"    \x1b[32m\" + m_ + \"\x1b[0m\") }\n"
    "  }\n"
    "}\n",
    name, name);

  pigeonInterpret(vm, "repl", src);
}

static bool handleCommand(const char* line, PigeonVM** vm)
{
  if (strcmp(line, ".help") == 0 || strcmp(line, ".h") == 0)
  {
    printHelp();
    return true;
  }
  else if (strcmp(line, ".quit") == 0 || strcmp(line, ".exit") == 0 || strcmp(line, ".q") == 0)
  {
    printf("%sBye!%s\n", COLOR_INFO, COLOR_RESET);
    return false;
  }
  else if (strcmp(line, ".clear") == 0 || strcmp(line, ".cls") == 0)
  {
    printf("\x1b[2J\x1b[H");
    printBanner();
    return true;
  }
  else if (strcmp(line, ".vars") == 0)
  {
    cmdVars(*vm);
    return true;
  }
  else if (strncmp(line, ".inspect", 8) == 0 && (line[8] == ' ' || line[8] == '\t'))
  {
    const char* name = line + 9;
    while (*name == ' ' || *name == '\t') name++;
    if (*name == '\0')
      printf("%sUsage: .inspect <name>%s\n", COLOR_INFO, COLOR_RESET);
    else
      cmdInspect(*vm, name);
    return true;
  }
  else if (strcmp(line, ".reset") == 0)
  {
    printf("%sResetting VM...%s\n", COLOR_INFO, COLOR_RESET);
    pigeonFreeVM(*vm);
    PigeonConfiguration config;
    replInitConfig(&config);
    *vm = pigeonNewVM(&config);
    printf("%sVM reset complete%s\n", COLOR_INFO, COLOR_RESET);
    return true;
  }

  return true;
}

static void executeCode(PigeonVM* vm, const char* code)
{

  // Check if it's a simple expression (no semicolon, no keywords that make it a statement)
  bool isExpression = true;
  const char* keywords[] = {"var", "class", "if", "for", "while", "import", "return", "break", "continue"};

  // Trim leading whitespace
  const char* trimmed = code;
  while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

  for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
  {
    size_t kwlen = strlen(keywords[i]);
    if (strncmp(trimmed, keywords[i], kwlen) == 0 &&
        (trimmed[kwlen] == ' ' || trimmed[kwlen] == '\t' || trimmed[kwlen] == '\0'))
    {
      isExpression = false;
      break;
    }
  }

  // Also not an expression if it ends with certain characters or contains blocks
  if (strchr(code, ';') != NULL || strchr(code, '{') != NULL)
  {
    isExpression = false;
  }

  if (isExpression && strlen(trimmed) > 0)
  {
    // Try to evaluate and print the expression
    char wrapped[MAX_LINE_LENGTH * 10 + 100];
    snprintf(wrapped, sizeof(wrapped), "System.print(%s)", code);

    PigeonInterpretResult result = pigeonInterpret(vm, "repl", wrapped);

    // If that failed, just execute it normally
    if (result == PIGEON_RESULT_COMPILE_ERROR)
    {
      pigeonInterpret(vm, "repl", code);
    }
  }
  else
  {
    pigeonInterpret(vm, "repl", code);
  }
}

static int runFile(PigeonVM* vm, const char* path)
{
  FILE* f = fopen(path, "r");
  if (!f)
  {
    fprintf(stderr, "%sError: Could not open file %s%s\n",
            COLOR_ERROR, path, COLOR_RESET);
    return 66; /* EX_NOINPUT */
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (size < 0)
  {
    fclose(f);
    fprintf(stderr, "%sError: Could not read file %s%s\n",
            COLOR_ERROR, path, COLOR_RESET);
    return 66; /* EX_NOINPUT */
  }
  fseek(f, 0, SEEK_SET);

  char* code = malloc((size_t)size + 1);
  if (!code)
  {
    fclose(f);
    fprintf(stderr, "%sError: Out of memory reading %s%s\n",
            COLOR_ERROR, path, COLOR_RESET);
    return 70; /* EX_SOFTWARE */
  }

  size_t nread = fread(code, 1, (size_t)size, f);
  code[nread] = '\0';
  fclose(f);

  PigeonInterpretResult result = pigeonInterpret(vm, path, code);
  free(code);

  if (result == PIGEON_RESULT_COMPILE_ERROR) return 65; /* EX_DATAERR */
  if (result == PIGEON_RESULT_RUNTIME_ERROR) return 70; /* EX_SOFTWARE */
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc > 1 && (strcmp(argv[1], "--version") == 0 ||
                   strcmp(argv[1], "-v") == 0))
  {
    printf("pigeon %s\n", PIGEON_VERSION_STRING);
    return 0;
  }

  if (argc > 1 && (strcmp(argv[1], "--help") == 0 ||
                   strcmp(argv[1], "-h") == 0))
  {
    printf("Usage: pigeon [file]\n");
    printf("  pigeon           Start the interactive REPL\n");
    printf("  pigeon <file>    Run a Pigeon source file and exit\n");
    printf("  pigeon --version Print the version and exit\n");
    return 0;
  }

  PigeonConfiguration config;
  replInitConfig(&config);
  PigeonVM* vm = pigeonNewVM(&config);

  // File argument: run and exit (the CLI, not a REPL session).
  if (argc > 1)
  {
    int status = runFile(vm, argv[1]);
    pigeonFreeVM(vm);
    return status;
  }

  printBanner();

  // Main REPL loop
  input_buffer[0] = '\0';

  while (true)
  {
    const char* prompt = in_multiline ? "....>" : "pigeon>";
    char* line = readLine(prompt);

    if (line == NULL)
    {
      printf("\n");
      break;
    }

    // Skip empty lines
    if (strlen(line) == 0 && !in_multiline)
    {
      continue;
    }

    // Handle commands
    if (line[0] == '.' && !in_multiline)
    {
      if (!handleCommand(line, &vm))
      {
        break;
      }
      continue;
    }

    // Build up multi-line input
    if (in_multiline)
    {
      size_t cur = strlen(input_buffer);
      size_t add = strlen(line) + 1; // +1 for the newline
      if (cur + add < sizeof(input_buffer))
      {
        strcat(input_buffer, "\n");
        strcat(input_buffer, line);
      }
    }
    else
    {
      strncpy(input_buffer, line, sizeof(input_buffer) - 1);
      input_buffer[sizeof(input_buffer) - 1] = '\0';
    }

    // Check if we need more lines
    if (isIncompleteBlock(input_buffer))
    {
      in_multiline = true;
      continue;
    }

    // Execute the code
    in_multiline = false;
    addToHistory(input_buffer);
    executeCode(vm, input_buffer);

    input_buffer[0] = '\0';
  }

  pigeonFreeVM(vm);
  return 0;
}
