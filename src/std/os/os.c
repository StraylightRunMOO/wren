#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include "os.h"
#include "wren.h"
#include "wren_common.h"

// Populated by wrenOsSetArgs() at startup.
static int   g_argc = 0;
static char** g_argv = NULL;

void wrenOsSetArgs(int argc, char** argv) {
  g_argc = argc;
  g_argv = argv;
}

// os.args_()
static void osArgs(WrenVM* vm) {
  wrenSetSlotNewList(vm, 0);
  for (int i = 0; i < g_argc; i++) {
    wrenSetSlotString(vm, 1, g_argv[i]);
    wrenInsertInList(vm, 0, -1, 1);
  }
}

// os.env_(key)
static void osEnv(WrenVM* vm) {
  const char* key = wrenGetSlotString(vm, 1);
  const char* val = getenv(key);
  if (val) wrenSetSlotString(vm, 0, val);
  else     wrenSetSlotNull(vm, 0);
}

// os.exit_(code)
static void osExit(WrenVM* vm) {
  int code = (int)wrenGetSlotDouble(vm, 1);
  exit(code);
}

#include "os.wren.inc"

const char* wrenOsSource() {
  return osModuleSource;
}

WrenForeignMethodFn wrenOsBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature)
{
  if (!isStatic) return NULL;
  if (strcmp(className, "os") == 0) {
    if (strcmp(signature, "args_()") == 0)   return osArgs;
    if (strcmp(signature, "env_(_)") == 0)   return osEnv;
    if (strcmp(signature, "exit_(_)") == 0)  return osExit;
  }
  return NULL;
}

WrenForeignClassMethods wrenOsBindForeignClass(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* WREN_MAYBE_UNUSED className)
{
  WrenForeignClassMethods methods = { NULL, NULL };
  return methods;
}
