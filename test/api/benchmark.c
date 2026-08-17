#include <string.h>
#include <time.h>

#include "benchmark.h"

static void arguments(PigeonVM* vm)
{
  double result = 0;

  result += pigeonGetSlotDouble(vm, 1);
  result += pigeonGetSlotDouble(vm, 2);
  result += pigeonGetSlotDouble(vm, 3);
  result += pigeonGetSlotDouble(vm, 4);

  pigeonSetSlotDouble(vm, 0, result);
}

const char* testScript =
"class Test {\n"
"  static method(a, b, c, d) { a + b + c + d }\n"
"}\n";

static void call(PigeonVM* vm)
{
  int iterations = (int)pigeonGetSlotDouble(vm, 1);

  // Since the VM is not re-entrant, we can't call from within this foreign
  // method. Instead, make a new VM to run the call test in.
  PigeonConfiguration config;
  pigeonInitConfiguration(&config);
  PigeonVM* otherVM = pigeonNewVM(&config);

  pigeonInterpret(otherVM, "main", testScript);

  PigeonHandle* method = pigeonMakeCallHandle(otherVM, "method(_,_,_,_)");

  pigeonEnsureSlots(otherVM, 1);
  pigeonGetVariable(otherVM, "main", "Test", 0);
  PigeonHandle* testClass = pigeonGetSlotHandle(otherVM, 0);

  double startTime = (double)clock() / CLOCKS_PER_SEC;

  double result = 0;
  for (int i = 0; i < iterations; i++)
  {
    pigeonEnsureSlots(otherVM, 5);
    pigeonSetSlotHandle(otherVM, 0, testClass);
    pigeonSetSlotDouble(otherVM, 1, 1.0);
    pigeonSetSlotDouble(otherVM, 2, 2.0);
    pigeonSetSlotDouble(otherVM, 3, 3.0);
    pigeonSetSlotDouble(otherVM, 4, 4.0);

    pigeonCall(otherVM, method);

    result += pigeonGetSlotDouble(otherVM, 0);
  }

  double elapsed = (double)clock() / CLOCKS_PER_SEC - startTime;

  pigeonReleaseHandle(otherVM, testClass);
  pigeonReleaseHandle(otherVM, method);
  pigeonFreeVM(otherVM);

  if (result == (1.0 + 2.0 + 3.0 + 4.0) * iterations)
  {
    pigeonSetSlotDouble(vm, 0, elapsed);
  }
  else
  {
    // Got the wrong result.
    pigeonSetSlotBool(vm, 0, false);
  }
}

PigeonForeignMethodFn benchmarkBindMethod(const char* signature)
{
  if (strcmp(signature, "static Benchmark.arguments(_,_,_,_)") == 0) return arguments;
  if (strcmp(signature, "static Benchmark.call(_)") == 0) return call;

  return NULL;
}
