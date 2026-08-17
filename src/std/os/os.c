#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#include "os.h"
#include "pigeon.h"
#include "wren_common.h"

// Populated by pigeonOsSetArgs() at startup.
static int   g_argc = 0;
static char** g_argv = NULL;

void pigeonOsSetArgs(int argc, char** argv) {
  g_argc = argc;
  g_argv = argv;
}

// os.args_()
static void osArgs(PigeonVM* vm) {
  pigeonSetSlotNewList(vm, 0);
  for (int i = 0; i < g_argc; i++) {
    pigeonSetSlotString(vm, 1, g_argv[i]);
    pigeonInsertInList(vm, 0, -1, 1);
  }
}

// os.env_(key)
static void osEnv(PigeonVM* vm) {
  const char* key = pigeonGetSlotString(vm, 1);
  const char* val = getenv(key);
  if (val) pigeonSetSlotString(vm, 0, val);
  else     pigeonSetSlotNull(vm, 0);
}

// os.exit_(code)
static void osExit(PigeonVM* vm) {
  int code = (int)pigeonGetSlotDouble(vm, 1);
  exit(code);
}

#include "os.wren.inc"

const char* pigeonOsSource() {
  return osModuleSource;
}

PigeonForeignMethodFn pigeonOsBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
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

PigeonForeignClassMethods pigeonOsBindForeignClass(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                               const char* PIGEON_MAYBE_UNUSED className)
{
  PigeonForeignClassMethods methods = { NULL, NULL };
  return methods;
}
