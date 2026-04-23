#include <stdio.h>
#include <string.h>

#include "wren_stdlib.h"

// Registry of stdlib modules
typedef struct {
  const char* name;
  const char* (*sourceFn)();
  WrenForeignMethodFn (*bindMethodFn)(WrenVM*, const char*, bool, const char*);
  WrenForeignClassMethods (*bindClassFn)(WrenVM*, const char*);
} StdlibModule;

static StdlibModule stdlibModules[] = {
  { "io", wrenIoSource, wrenIoBindForeignMethod, NULL },
  { "strconv", wrenStrconvSource, wrenStrconvBindForeignMethod, NULL },
  { "time", wrenTimeSource, wrenTimeBindForeignMethod, NULL },
  { "math", wrenMathModuleSource, wrenMathModuleBindForeignMethod, NULL },
  { "math/random", wrenRandomModuleSource, wrenRandomModuleBindForeignMethod, wrenRandomModuleBindForeignClass },
  { "strings", wrenStringsSource, wrenStringsBindForeignMethod, NULL },
  { NULL, NULL, NULL, NULL }
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
