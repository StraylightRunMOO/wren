#include "./test.h"
#include "./api/api_tests.h"
#include "./api/object_number.h"
#include "../src/std/wren_stdlib.h"

#include <stdio.h>
#include <string.h>

static WrenVM* vm = NULL;

//This is a simple test runner that serves one purpose:
//To run the language level tests and benchmarks for Wren.
//It is not a general purpose vm or REPL.
//See wren-cli if you're looking for that.

// Callback for default imports (e.g., `import "math"`)
static const char* resolveDefaultExport(WrenVM* vm, const char* name)
{
  (void)vm;
  return wrenStdlibGetDefaultExport(name);
}

// Callback for wildcard imports (e.g., `import "math" for *`)
static const char** resolveExports(WrenVM* vm, const char* name)
{
  (void)vm;
  return wrenStdlibGetExports(name);
}

static WrenVM* initVM(bool isAPITest)
{
  WrenConfiguration config;
  wrenInitConfiguration(&config);

  config.resolveModuleFn = resolveModule;
  config.loadModuleFn = readModule;
  config.writeFn = vm_write;
  config.errorFn = reportError;
  config.resolveDefaultExportFn = resolveDefaultExport;
  config.resolveExportsFn = resolveExports;

  if(isAPITest) {
    config.bindForeignClassFn = APITest_bindForeignClass;
    config.bindForeignMethodFn = APITest_bindForeignMethod;
    config.objectNumberFn = objectNumberGetCallback();
  }

  // Since we're running in a standalone process, be generous with memory.
  config.initialHeapSize = 1024 * 1024 * 100;
  return wrenNewVM(&config);
}

int main(int argc, const char* argv[]) {

  int handled = handle_args(argc, argv);
  if(handled != 0) return handled;

  int exitCode = 0;
  const char* testName = argv[1];
  bool isAPITest = isModuleAnAPITest(testName);

  vm = initVM(isAPITest);
  WrenInterpretResult result = runFile(vm, testName);

  if(isAPITest) {
    exitCode = APITest_Run(vm, testName);
  }

  if (result == WREN_RESULT_COMPILE_ERROR) return WREN_EX_DATAERR;
  if (result == WREN_RESULT_RUNTIME_ERROR) return WREN_EX_SOFTWARE;

  wrenFreeVM(vm);

  return exitCode;

}

