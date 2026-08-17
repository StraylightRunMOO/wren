#include <stdio.h>
#include <string.h>

#include "call.h"

int callRunTests(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonGetVariable(vm, "./test/api/call", "Call", 0);
  PigeonHandle* callClass = pigeonGetSlotHandle(vm, 0);

  PigeonHandle* noParams = pigeonMakeCallHandle(vm, "noParams");
  PigeonHandle* zero = pigeonMakeCallHandle(vm, "zero()");
  PigeonHandle* one = pigeonMakeCallHandle(vm, "one(_)");
  PigeonHandle* two = pigeonMakeCallHandle(vm, "two(_,_)");
  PigeonHandle* unary = pigeonMakeCallHandle(vm, "-");
  PigeonHandle* binary = pigeonMakeCallHandle(vm, "-(_)");
  PigeonHandle* subscript = pigeonMakeCallHandle(vm, "[_,_]");
  PigeonHandle* subscriptSet = pigeonMakeCallHandle(vm, "[_,_]=(_)");

  // Different arity.
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonCall(vm, noParams);

  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonCall(vm, zero);

  pigeonEnsureSlots(vm, 2);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonCall(vm, one);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 2.0);
  pigeonCall(vm, two);

  // Operators.
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonCall(vm, unary);

  pigeonEnsureSlots(vm, 2);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonCall(vm, binary);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 2.0);
  pigeonCall(vm, subscript);

  pigeonEnsureSlots(vm, 4);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 2.0);
  pigeonSetSlotDouble(vm, 3, 3.0);
  pigeonCall(vm, subscriptSet);

  // Returning a value.
  PigeonHandle* getValue = pigeonMakeCallHandle(vm, "getValue()");
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonCall(vm, getValue);
  printf("slots after call: %d\n", pigeonGetSlotCount(vm));
  PigeonHandle* value = pigeonGetSlotHandle(vm, 0);

  // Different argument types.
  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotBool(vm, 1, true);
  pigeonSetSlotBool(vm, 2, false);
  pigeonCall(vm, two);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotDouble(vm, 1, 1.2);
  pigeonSetSlotDouble(vm, 2, 3.4);
  pigeonCall(vm, two);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotString(vm, 1, "string");
  pigeonSetSlotString(vm, 2, "another");
  pigeonCall(vm, two);

  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotNull(vm, 1);
  pigeonSetSlotHandle(vm, 2, value);
  pigeonCall(vm, two);

  // Truncate a string, or allow null bytes.
  pigeonEnsureSlots(vm, 3);
  pigeonSetSlotHandle(vm, 0, callClass);
  pigeonSetSlotBytes(vm, 1, "string", 3);
  pigeonSetSlotBytes(vm, 2, "b\0y\0t\0e", 7);
  pigeonCall(vm, two);

  // Call ignores with extra temporary slots on stack.
  pigeonEnsureSlots(vm, 10);
  pigeonSetSlotHandle(vm, 0, callClass);
  for (int i = 1; i < 10; i++)
  {
    pigeonSetSlotDouble(vm, i, i * 0.1);
  }
  pigeonCall(vm, one);

  pigeonReleaseHandle(vm, callClass);
  pigeonReleaseHandle(vm, noParams);
  pigeonReleaseHandle(vm, zero);
  pigeonReleaseHandle(vm, one);
  pigeonReleaseHandle(vm, two);
  pigeonReleaseHandle(vm, getValue);
  pigeonReleaseHandle(vm, value);
  pigeonReleaseHandle(vm, unary);
  pigeonReleaseHandle(vm, binary);
  pigeonReleaseHandle(vm, subscript);
  pigeonReleaseHandle(vm, subscriptSet);

  return 0;
}
