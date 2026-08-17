#include <string.h>

#include "new_vm.h"

static void nullConfig(PigeonVM* vm)
{
  PigeonVM* otherVM = pigeonNewVM(NULL);

  // We should be able to execute code.
  PigeonInterpretResult result = pigeonInterpret(otherVM, "main", "1 + 2");
  pigeonSetSlotBool(vm, 0, result == PIGEON_RESULT_SUCCESS);

  pigeonFreeVM(otherVM);
}

static void multipleInterpretCalls(PigeonVM* vm)
{
  PigeonVM* otherVM = pigeonNewVM(NULL);
  PigeonInterpretResult result;

  bool correct = true;

  // Handles should be valid across calls into Wren code.
  PigeonHandle* absMethod = pigeonMakeCallHandle(otherVM, "abs");

  result = pigeonInterpret(otherVM, "main", "import \"random\" for Random");
  correct = correct && (result == PIGEON_RESULT_SUCCESS);

  for (int i = 0; i < 5; i++) {
    // Calling `pigeonEnsureSlots()` before `pigeonInterpret()` should not introduce
    // problems later.
    pigeonEnsureSlots(otherVM, 2);

    // Calling a foreign function should succeed.
    result = pigeonInterpret(otherVM, "main", "Random.new(12345)");
    correct = correct && (result == PIGEON_RESULT_SUCCESS);

    pigeonEnsureSlots(otherVM, 2);
    pigeonSetSlotDouble(otherVM, 0, -i);
    result = pigeonCall(otherVM, absMethod);
    correct = correct && (result == PIGEON_RESULT_SUCCESS);

    double absValue = pigeonGetSlotDouble(otherVM, 0);
    correct = correct && (absValue == (double)i);
  }

  pigeonSetSlotBool(vm, 0, correct);

  pigeonReleaseHandle(otherVM, absMethod);
  pigeonFreeVM(otherVM);
}

PigeonForeignMethodFn newVMBindMethod(const char* signature)
{
  if (strcmp(signature, "static VM.nullConfig()") == 0) return nullConfig;
  if (strcmp(signature, "static VM.multipleInterpretCalls()") == 0) return multipleInterpretCalls;

  return NULL;
}
