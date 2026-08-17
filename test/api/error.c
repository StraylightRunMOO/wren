#include <stdio.h>
#include <string.h>

#include "error.h"

static void runtimeError(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotString(vm, 0, "Error!");
  pigeonAbortFiber(vm, 0);
}

PigeonForeignMethodFn errorBindMethod(const char* signature)
{
  if (strcmp(signature, "static Error.runtimeError") == 0) return runtimeError;

  return NULL;
}
