#include <stdio.h>
#include <string.h>

#include "pigeon.h"
#include "../test.h"

int callWrenCallRootRunTests(PigeonVM* vm)
{
  int exitCode = 0;
  pigeonEnsureSlots(vm, 1);
  pigeonGetVariable(vm, "./test/api/call_wren_call_root", "Test", 0);
  PigeonHandle* testClass = pigeonGetSlotHandle(vm, 0);

  PigeonHandle* run = pigeonMakeCallHandle(vm, "run()");

  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, testClass);
  PigeonInterpretResult result = pigeonCall(vm, run);
  if (result == PIGEON_RESULT_RUNTIME_ERROR)
  {
    exitCode = PIGEON_EX_SOFTWARE;
  }
  else
  {
    printf("Missing runtime error.\n");
  }

  pigeonReleaseHandle(vm, testClass);
  pigeonReleaseHandle(vm, run);
  return exitCode;
}
