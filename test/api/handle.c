#include <string.h>

#include "handle.h"

static PigeonHandle* handle;

static void setValue(PigeonVM* vm)
{
  handle = pigeonGetSlotHandle(vm, 1);
}

static void getValue(PigeonVM* vm)
{
  pigeonSetSlotHandle(vm, 0, handle);
  pigeonReleaseHandle(vm, handle);
}

PigeonForeignMethodFn handleBindMethod(const char* signature)
{
  if (strcmp(signature, "static Handle.value=(_)") == 0) return setValue;
  if (strcmp(signature, "static Handle.value") == 0) return getValue;

  return NULL;
}
