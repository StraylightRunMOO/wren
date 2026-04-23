// Wren REPL - Interactive Read-Eval-Print Loop
// A simple but nice REPL for experimenting with Wren code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "wren.h"

// Forward declarations for stdlib functions (avoids pulling in internal VM headers)
const char*  wrenStdlibLoadModule(const char* name);
const char*  wrenStdlibGetDefaultExport(const char* name);
const char** wrenStdlibGetExports(const char* name);
WrenForeignMethodFn wrenStdlibBindForeign(WrenVM* vm, const char* module,
    const char* className, bool isStatic, const char* signature);
WrenForeignClassMethods wrenStdlibBindForeignClass(WrenVM* vm,
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


static void writeFn(WrenVM* vm, const char* text)
{
  printf("%s%s%s", COLOR_OUTPUT, text, COLOR_RESET);
}

static void errorFn(WrenVM* vm, WrenErrorType errorType,
                    const char* module, const int line,
                    const char* msg)
{
  switch (errorType)
  {
    case WREN_ERROR_COMPILE:
      fprintf(stderr, "%s[%s line %d] Error: %s%s\n",
              COLOR_ERROR, module, line, msg, COLOR_RESET);
      break;
    case WREN_ERROR_STACK_TRACE:
      fprintf(stderr, "%s[%s line %d] in %s%s\n",
              COLOR_ERROR, module, line, msg, COLOR_RESET);
      break;
    case WREN_ERROR_RUNTIME:
      fprintf(stderr, "%s[Runtime Error] %s%s\n",
              COLOR_ERROR, msg, COLOR_RESET);
      break;
  }
}

static void printBanner(void)
{
  printf("%s", COLOR_INFO);
  printf("╔═══════════════════════════════════════════════════════════╗\n");
  printf("║                     Wren REPL v0.4.0                      ║\n");
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
  printf("\nWren REPL Commands:\n");
  printf("  .help            - Show this help message\n");
  printf("  .quit / .exit    - Exit the REPL\n");
  printf("  .clear / .cls    - Clear the screen\n");
  printf("  .vars            - List variables defined in the REPL session\n");
  printf("  .inspect <name>  - Show type, class, and methods for a variable or class\n");
  printf("  .reset           - Reset the VM and clear all state\n");
  printf("\nExamples:\n");
  printf("  wren> 2 + 2\n");
  printf("  wren> var x = 10\n");
  printf("  wren> System.print(\"Hello, World!\")\n");
  printf("  wren> [1, 2, 3].map {|n| n * 2 }\n");
  printf("  wren> \"\"\"                    // Start multi-line\n");
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

static const char* replResolveModule(WrenVM* vm, const char* importer,
                                     const char* module)
{
  (void)vm; (void)importer;
  return module;
}

static void replLoadModuleComplete(WrenVM* vm, const char* module,
                                   WrenLoadModuleResult result)
{
  (void)vm; (void)module;
  if (result.source) free((void*)result.source);
}

static WrenLoadModuleResult replLoadModule(WrenVM* vm, const char* module)
{
  WrenLoadModuleResult result = {0};

  // Try stdlib first.
  const char* src = wrenStdlibLoadModule(module);
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
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    result.source = buf;
    result.onComplete = replLoadModuleComplete;
  }
  return result;
}

static WrenForeignMethodFn replBindForeignMethod(WrenVM* vm,
    const char* module, const char* className, bool isStatic,
    const char* signature)
{
  return wrenStdlibBindForeign(vm, module, className, isStatic, signature);
}

static WrenForeignClassMethods replBindForeignClass(WrenVM* vm,
    const char* module, const char* className)
{
  return wrenStdlibBindForeignClass(vm, module, className);
}

static const char* replResolveDefaultExport(WrenVM* vm, const char* name)
{
  (void)vm;
  return wrenStdlibGetDefaultExport(name);
}

static const char** replResolveExports(WrenVM* vm, const char* name)
{
  (void)vm;
  return wrenStdlibGetExports(name);
}

static void replInitConfig(WrenConfiguration* config)
{
  wrenInitConfiguration(config);
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

static void cmdVars(WrenVM* vm)
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
  wrenInterpret(vm, mod, src);
}

static void cmdInspect(WrenVM* vm, const char* name)
{
  char mod[64];
  snprintf(mod, sizeof(mod), "repl.cmd%d", g_cmdSeq++);

  char src[4096];
  snprintf(src, sizeof(src),
    "import \"meta\" for meta\n"
    "import \"meta/reflection\" for reflection\n"
    "var expr = meta.compileExpression(\"%s\")\n"
    "var val  = expr == null ? null : expr.call()\n"
    "if (val == null) {\n"
    "  System.print(\"\x1b[31mCould not evaluate: %s\x1b[0m\")\n"
    "} else {\n"
    "  var isClass = val is Class\n"
    "  var cls = isClass ? reflection.getClass(val.name) : reflection.classOf(val)\n"
    "  if (cls == null) {\n"
    "    System.print(\"\x1b[33m%s\x1b[0m : \x1b[36m%%(val.type.name)\x1b[0m\")\n"
    "  } else {\n"
    "    var tag = isClass ? \"class\" : \"instance of\"\n"
    "    System.print(\"\x1b[33m%s\x1b[0m : \x1b[36m%%(tag) %%(cls.name)\x1b[0m\")\n"
    "    var instMethods = []\n"
    "    var staticMethods = []\n"
    "    for (mname in cls.methods) {\n"
    "      var mi = cls.methodInfo(mname)\n"
    "      if (mi.isStatic) { staticMethods.add(mname) } else { instMethods.add(mname) }\n"
    "    }\n"
    "    instMethods.sort()\n"
    "    staticMethods.sort()\n"
    "    if (staticMethods.count > 0) {\n"
    "      System.print(\"\x1b[90m  static:\x1b[0m\")\n"
    "      for (m in staticMethods) { System.print(\"    \x1b[32m%%(m)\x1b[0m\") }\n"
    "    }\n"
    "    if (instMethods.count > 0) {\n"
    "      System.print(\"\x1b[90m  instance:\x1b[0m\")\n"
    "      for (m in instMethods) { System.print(\"    \x1b[32m%%(m)\x1b[0m\") }\n"
    "    }\n"
    "  }\n"
    "}\n",
    name, name, name, name);

  wrenInterpret(vm, mod, src);
}

static bool handleCommand(const char* line, WrenVM** vm)
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
    wrenFreeVM(*vm);
    WrenConfiguration config;
    replInitConfig(&config);
    *vm = wrenNewVM(&config);
    printf("%sVM reset complete%s\n", COLOR_INFO, COLOR_RESET);
    return true;
  }

  return true;
}

static void executeCode(WrenVM* vm, const char* code)
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

    WrenInterpretResult result = wrenInterpret(vm, "repl", wrapped);

    // If that failed, just execute it normally
    if (result == WREN_RESULT_COMPILE_ERROR)
    {
      wrenInterpret(vm, "repl", code);
    }
  }
  else
  {
    wrenInterpret(vm, "repl", code);
  }
}

int main(int argc, char* argv[])
{
  // Initialize Wren VM
  WrenConfiguration config;
  replInitConfig(&config);
  WrenVM* vm = wrenNewVM(&config);

  // Print banner
  printBanner();

  // Execute any file passed as argument
  if (argc > 1)
  {
    FILE* f = fopen(argv[1], "r");
    if (f)
    {
      fseek(f, 0, SEEK_END);
      long size = ftell(f);
      fseek(f, 0, SEEK_SET);

      char* code = malloc(size + 1);
      fread(code, 1, size, f);
      code[size] = '\0';
      fclose(f);

      printf("%sExecuting %s...%s\n\n", COLOR_INFO, argv[1], COLOR_RESET);
      wrenInterpret(vm, argv[1], code);
      free(code);

      printf("\n");
    }
    else
    {
      fprintf(stderr, "%sError: Could not open file %s%s\n", COLOR_ERROR, argv[1], COLOR_RESET);
    }
  }

  // Main REPL loop
  input_buffer[0] = '\0';

  while (true)
  {
    const char* prompt = in_multiline ? "....>" : "wren>";
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
      strcat(input_buffer, "\n");
      strcat(input_buffer, line);
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

  wrenFreeVM(vm);
  return 0;
}
