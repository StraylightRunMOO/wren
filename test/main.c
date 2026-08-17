#include "./test.h"
#include "./api/api_tests.h"
#include "./api/object_number.h"
#include "../src/std/wren_stdlib.h"

#include <stdio.h>
#include <string.h>

static PigeonVM* vm = NULL;

// Test runner for the Pigeon language suite and C API tests.
// Not a general-purpose VM. Use the `pigeon` CLI for that.

// Callback for default imports (e.g., `import "math"`)
static const char* resolveDefaultExport(PigeonVM* vm, const char* name)
{
  (void)vm;
  return pigeonStdlibGetDefaultExport(name);
}

// Callback for wildcard imports (e.g., `import "math" for *`)
static const char** resolveExports(PigeonVM* vm, const char* name)
{
  (void)vm;
  return pigeonStdlibGetExports(name);
}

static PigeonVM* initVM(bool isAPITest)
{
  PigeonConfiguration config;
  pigeonInitConfiguration(&config);

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
  return pigeonNewVM(&config);
}

int main(int argc, const char* argv[]) {

  int handled = handle_args(argc, argv);
  if(handled != 0) return handled;

  int exitCode = 0;
  const char* testName = argv[1];
  bool isAPITest = isModuleAnAPITest(testName);

  vm = initVM(isAPITest);
  PigeonInterpretResult result = runFile(vm, testName);

  if(isAPITest) {
    exitCode = APITest_Run(vm, testName);
  }

  if (result == PIGEON_RESULT_COMPILE_ERROR) return PIGEON_EX_DATAERR;
  if (result == PIGEON_RESULT_RUNTIME_ERROR) return PIGEON_EX_SOFTWARE;

  pigeonFreeVM(vm);

  return exitCode;

}

