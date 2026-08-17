#include <stdio.h>
#include <string.h>

#include "wren_stdlib.h"

// Registry of stdlib modules
typedef struct {
  const char* name;
  const char* (*sourceFn)();
  WrenForeignMethodFn (*bindMethodFn)(WrenVM*, const char*, bool, const char*);
  WrenForeignClassMethods (*bindClassFn)(WrenVM*, const char*);
  const char* defaultExport;     // Default export name (e.g., "Math" for "math")
  const char** exports;          // NULL-terminated list of public exports
} StdlibModule;

// Export lists for wildcard imports
static const char* ioExports[] = { "io", "Reader", "Writer", "Pipe", NULL };
static const char* fsExports[] = { "fs", "File", NULL };
static const char* osExports[] = { "os", "Stdin", "Stdout", "Stderr", NULL };
static const char* strconvExports[] = { "strconv", "Atoi", "Atof", "Itoa", "Ftoa", NULL };
static const char* timeExports[] = { "Time", "Duration", "Timer", "Now", "Sleep", NULL };
static const char* mathExports[] = { "math", "Rand", "Stats", "Inf", "NaN", "E", "Pi", "Phi", NULL };
static const char* mathRandomExports[] = { "Random", "RandomState", "ALG_XOSHIRO256PP", "ALG_XOROSHIRO128PP", "ALG_WYRAND", "ALG_XSHIFT64STAR", "Uint64", "Double", "Float", "Int", "IntInRange", "RandBool", "Normal", "NormalParams", "Choice", "Shuffle", "Sample", "Seed", "Pi", NULL };
static const char* stringsExports[] = { "strings", "Builder", "Contains", "HasPrefix", "HasSuffix", "Join", "Repeat", "Replace", "Split", "Trim", "ToLower", "ToUpper", NULL };
static const char* metaExports[] = { "meta", "Meta", NULL };
static const char* metaReflectionExports[] = { "reflection", "ClassInfo", "MethodInfo", "Reflection_", NULL };
static const char* jsonExports[] = { "Json", NULL };

static StdlibModule stdlibModules[] = {
  { "io", wrenIoSource, wrenIoBindForeignMethod, NULL, "io", ioExports },
  { "fs", wrenFsSource, wrenFsBindForeignMethod, NULL, "fs", fsExports },
  { "os", wrenOsSource, wrenOsBindForeignMethod, NULL, "os", osExports },
  { "strconv", wrenStrconvSource, wrenStrconvBindForeignMethod, NULL, "strconv", strconvExports },
  { "time", wrenTimeSource, wrenTimeBindForeignMethod, NULL, "Time", timeExports },
  { "math", wrenMathModuleSource, wrenMathModuleBindForeignMethod, NULL, "math", mathExports },
  { "math/random", wrenRandomModuleSource, wrenRandomModuleBindForeignMethod, wrenRandomModuleBindForeignClass, "Random", mathRandomExports },
  { "strings", wrenStringsSource, wrenStringsBindForeignMethod, NULL, "strings", stringsExports },
  { "meta", wrenMetaSource, wrenMetaBindForeignMethod, NULL, "meta", metaExports },
  { "meta/reflection", wrenReflectionSource, wrenReflectionBindForeignMethod, NULL, "reflection", metaReflectionExports },
  { "encoding/json", wrenJsonSource, wrenJsonBindForeignMethod, NULL, "Json", jsonExports },
  { NULL, NULL, NULL, NULL, NULL, NULL }
};

// Check if a module is a stdlib module
int wrenStdlibHasModule(const char* name) {
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, name) == 0) {
      return 1;
    }
  }
  return 0;
}

// Load module source
const char* wrenStdlibLoadModule(const char* name) {
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, name) == 0) {
      return stdlibModules[i].sourceFn();
    }
  }
  return NULL;
}

// Binds foreign methods for a module
WrenForeignMethodFn wrenStdlibBindForeign(WrenVM* vm, const char* module, 
                                          const char* className, bool isStatic,
                                          const char* signature)
{
  (void)vm;
  (void)className;
  (void)isStatic;
  (void)signature;
  
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, module) == 0) {
      return stdlibModules[i].bindMethodFn(vm, className, isStatic, signature);
    }
  }
  return NULL;
}

// Binds foreign classes for a module
WrenForeignClassMethods wrenStdlibBindForeignClass(WrenVM* vm, const char* module,
                                                   const char* className)
{
  (void)vm;
  (void)className;
  
  WrenForeignClassMethods methods = { NULL, NULL };
  
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, module) == 0) {
      if (stdlibModules[i].bindClassFn != NULL) {
        return stdlibModules[i].bindClassFn(vm, className);
      }
      break;
    }
  }
  return methods;
}

// Get the default export name for a module (e.g., "Math" for "math")
// Returns NULL if module not found or has no default export
const char* wrenStdlibGetDefaultExport(const char* name) {
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, name) == 0) {
      return stdlibModules[i].defaultExport;
    }
  }
  return NULL;
}

// Get the list of exports for a module (for wildcard imports)
// Returns NULL if module not found
const char** wrenStdlibGetExports(const char* name) {
  for (int i = 0; stdlibModules[i].name != NULL; i++) {
    if (strcmp(stdlibModules[i].name, name) == 0) {
      return stdlibModules[i].exports;
    }
  }
  return NULL;
}
