#include <stdio.h>
#include <string.h>

#include "pigeon.h"

static void counterAllocate(PigeonVM* vm)
{
  double* counter = (double*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(double));
  *counter = pigeonGetSlotDouble(vm, 1);
}

void resetStackAfterForeignConstructBindClass(
    const char* className, PigeonForeignClassMethods* methods)
{
  if (strcmp(className, "ResetStackForeign") == 0)
  {
    methods->allocate = counterAllocate;
    return;
  }
}

int resetStackAfterForeignConstructRunTests(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonGetVariable(vm,
      "./test/api/reset_stack_after_foreign_construct", "Test", 0);
  PigeonHandle* testClass = pigeonGetSlotHandle(vm, 0);

  PigeonHandle* callConstruct = pigeonMakeCallHandle(vm, "callConstruct()");
  PigeonHandle* afterConstruct = pigeonMakeCallHandle(vm, "afterConstruct(_,_)");

  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, testClass);
  pigeonCall(vm, callConstruct);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, testClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 2.0);
  pigeonCall(vm, afterConstruct);

  pigeonReleaseHandle(vm, testClass);
  pigeonReleaseHandle(vm, callConstruct);
  pigeonReleaseHandle(vm, afterConstruct);

  return 0;
}
