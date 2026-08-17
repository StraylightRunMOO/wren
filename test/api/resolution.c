#include <stdio.h>
#include <string.h>

#include "resolution.h"

static void writeFn(PigeonVM* vm, const char* text)
{
  printf("%s", text);
}

static void reportError(PigeonVM* vm, PigeonErrorType type,
                        const char* module, int line, const char* message)
{
  if (type == PIGEON_ERROR_RUNTIME) printf("%s\n", message);
}

static void loadModuleComplete(PigeonVM* vm, const char* module, PigeonLoadModuleResult result)
{
  free((void*)result.source);
}

static PigeonLoadModuleResult loadModule(PigeonVM* vm, const char* module)
{
  printf("loading %s\n", module);

  const char* source;
  if (strcmp(module, "main/baz/bang") == 0)
  {
    source = "import \"foo|bar\"";
  }
  else
  {
    source = "System.print(\"ok\")";
  }
   
  char* string = (char*)malloc(strlen(source) + 1);
  strcpy(string, source);

  PigeonLoadModuleResult result = {0};
    result.onComplete = loadModuleComplete;
    result.source = string;
  return result;
}

static void runTestVM(PigeonVM* vm, PigeonConfiguration* configuration,
                      const char* source)
{
  configuration->writeFn = writeFn;
  configuration->errorFn = reportError;
  configuration->loadModuleFn = loadModule;

  PigeonVM* otherVM = pigeonNewVM(configuration);

  // We should be able to execute code.
  PigeonInterpretResult result = pigeonInterpret(otherVM, "main", source);
  if (result != PIGEON_RESULT_SUCCESS)
  {
    pigeonSetSlotString(vm, 0, "error");
  }
  else
  {
    pigeonSetSlotString(vm, 0, "success");
  }

  pigeonFreeVM(otherVM);
}

static void noResolver(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  // Should default to no resolution function.
  if (configuration.resolveModuleFn != NULL)
  {
    pigeonSetSlotString(vm, 0, "Did not have null resolve function.");
    return;
  }

  runTestVM(vm, &configuration, "import \"foo/bar\"");
}

static const char* resolveToNull(PigeonVM* vm, const char* importer,
                                 const char* name)
{
  return NULL;
}

static void returnsNull(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  configuration.resolveModuleFn = resolveToNull;
  runTestVM(vm, &configuration, "import \"foo/bar\"");
}

static const char* resolveChange(PigeonVM* vm, const char* importer,
                                 const char* name)
{
  // Concatenate importer and name.
  size_t length = strlen(importer) + 1 + strlen(name) + 1;
  char* result = (char*)malloc(length);
  strcpy(result, importer);
  strcat(result, "/");
  strcat(result, name);

  // Replace "|" with "/".
  for (size_t i = 0; i < length; i++)
  {
    if (result[i] == '|') result[i] = '/';
  }

  return result;
}

static void changesString(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  configuration.resolveModuleFn = resolveChange;
  runTestVM(vm, &configuration, "import \"foo|bar\"");
}

static void shared(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  configuration.resolveModuleFn = resolveChange;
  runTestVM(vm, &configuration, "import \"foo|bar\"\nimport \"foo/bar\"");
}

static void importer(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  configuration.resolveModuleFn = resolveChange;
  runTestVM(vm, &configuration, "import \"baz|bang\"");
}

PigeonForeignMethodFn resolutionBindMethod(const char* signature)
{
  if (strcmp(signature, "static Resolution.noResolver()") == 0) return noResolver;
  if (strcmp(signature, "static Resolution.returnsNull()") == 0) return returnsNull;
  if (strcmp(signature, "static Resolution.changesString()") == 0) return changesString;
  if (strcmp(signature, "static Resolution.shared()") == 0) return shared;
  if (strcmp(signature, "static Resolution.importer()") == 0) return importer;

  return NULL;
}

void resolutionBindClass(const char* className, PigeonForeignClassMethods* methods)
{
//  methods->allocate = foreignClassAllocate;
}
