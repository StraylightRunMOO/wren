// Example demonstrating Object Number feature in Wren
// Object Numbers are hash-prefixed integer literals like #123
// that call a C callback during compilation to create Wren values.

#include <stdio.h>
#include <stdint.h>
#include "wren.h"

// Simple callback that converts Object Numbers to doubled values
static void objectNumberCallback(WrenVM* vm, int64_t value)
{
  wrenEnsureSlots(vm, 1);
  // Return double the value as a demonstration
  wrenSetSlotDouble(vm, 0, (double)(value * 2));
}

static void writeFn(WrenVM* vm, const char* text)
{
  printf("%s", text);
}

static void errorFn(WrenVM* vm, WrenErrorType errorType,
                    const char* module, const int line,
                    const char* msg)
{
  switch (errorType)
  {
    case WREN_ERROR_COMPILE:
      printf("[%s line %d] [Error] %s\n", module, line, msg);
      break;
    case WREN_ERROR_STACK_TRACE:
      printf("[%s line %d] in %s\n", module, line, msg);
      break;
    case WREN_ERROR_RUNTIME:
      printf("[Runtime Error] %s\n", msg);
      break;
  }
}

int main(int argc, const char* argv[])
{
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.writeFn = writeFn;
  config.errorFn = errorFn;
  config.objectNumberFn = objectNumberCallback;

  WrenVM* vm = wrenNewVM(&config);

  const char* script =
    "System.print(\"Object Number Demo\")\n"
    "System.print(\"==================\")\n"
    "\n"
    "// Object Numbers are resolved at compile-time\n"
    "// This callback doubles each value\n"
    "var a = #5\n"
    "var b = #10\n"
    "var c = #100\n"
    "\n"
    "System.print(\"#5 = %(a)\")    // 10\n"
    "System.print(\"#10 = %(b)\")   // 20\n"
    "System.print(\"#100 = %(c)\")  // 200\n"
    "\n"
    "// Can be used in expressions\n"
    "var sum = #1 + #2 + #3\n"
    "System.print(\"#1 + #2 + #3 = %(sum)\")  // 2 + 4 + 6 = 12\n"
    "\n"
    "// Can be used in data structures\n"
    "var list = [#7, #8, #9]\n"
    "System.print(\"List: %(list)\")  // [14, 16, 18]\n";

  WrenInterpretResult result = wrenInterpret(vm, "main", script);

  if (result == WREN_RESULT_SUCCESS)
  {
    printf("\n✓ Demo completed successfully\n");
  }
  else
  {
    printf("\n✗ Demo failed\n");
  }

  wrenFreeVM(vm);
  return result == WREN_RESULT_SUCCESS ? 0 : 1;
}
