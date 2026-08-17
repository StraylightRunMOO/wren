#include <stdio.h>
#include <string.h>

#include "pigeon.h"

int resetStackAfterCallAbortRunTests(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonGetVariable(vm, "./test/api/reset_stack_after_call_abort", "Test", 0);
  PigeonHandle* testClass = pigeonGetSlotHandle(vm, 0);

  PigeonHandle* abortFiber = pigeonMakeCallHandle(vm, "abortFiber()");
  PigeonHandle* afterAbort = pigeonMakeCallHandle(vm, "afterAbort(_,_)");

  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, testClass);
  pigeonCall(vm, abortFiber);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, testClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 2.0);
  pigeonCall(vm, afterAbort);

  pigeonReleaseHandle(vm, testClass);
  pigeonReleaseHandle(vm, abortFiber);
  pigeonReleaseHandle(vm, afterAbort);
  return 0;
}
