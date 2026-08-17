#include <stdio.h>
#include <string.h>

#include "pigeon.h"

static void api(PigeonVM *vm) {
  // Grow the slot array. This should trigger the stack to be moved.
  pigeonEnsureSlots(vm, 10);
  pigeonSetSlotNewList(vm, 0);

  for (int i = 1; i < 10; i++)
  {
    pigeonSetSlotDouble(vm, i, i);
    pigeonInsertInList(vm, 0, -1, i);
  }
}

PigeonForeignMethodFn callCallsForeignBindMethod(const char* signature)
{
  if (strcmp(signature, "static CallCallsForeign.api()") == 0) return api;

  return NULL;
}

int callCallsForeignRunTests(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonGetVariable(vm, "./test/api/call_calls_foreign", "CallCallsForeign", 0);
  PigeonHandle* apiClass = pigeonGetSlotHandle(vm, 0);
  PigeonHandle *call = pigeonMakeCallHandle(vm, "call(_)");

  pigeonEnsureSlots(vm, 2);
  pigeonSetSlotHandle(vm, 0, apiClass);
  pigeonSetSlotString(vm, 1, "parameter");

  printf("slots before %d\n", pigeonGetSlotCount(vm));
  pigeonCall(vm, call);

  // We should have a single slot count for the return.
  printf("slots after %d\n", pigeonGetSlotCount(vm));

  pigeonReleaseHandle(vm, call);
  pigeonReleaseHandle(vm, apiClass);
  return 0;
}
