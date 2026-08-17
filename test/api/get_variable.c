#include <string.h>

#include "get_variable.h"

static void beforeDefined(PigeonVM* vm)
{
  pigeonGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void afterDefined(PigeonVM* vm)
{
  pigeonGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void afterAssigned(PigeonVM* vm)
{
  pigeonGetVariable(vm, "./test/api/get_variable", "A", 0);
}

static void otherSlot(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 3);
  pigeonGetVariable(vm, "./test/api/get_variable", "B", 2);

  // Move it into return position.
  const char* string = pigeonGetSlotString(vm, 2);
  pigeonSetSlotString(vm, 0, string);
}

static void otherModule(PigeonVM* vm)
{
  pigeonGetVariable(vm, "./test/api/get_variable_module", "Variable", 0);
}

static void hasVariable(PigeonVM* vm)
{
  const char* module = pigeonGetSlotString(vm, 1);
  const char* variable = pigeonGetSlotString(vm, 2);

  bool result = pigeonHasVariable(vm, module, variable);
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotBool(vm, 0, result);
}

static void hasModule(PigeonVM* vm)
{
  const char* module = pigeonGetSlotString(vm, 1);

  bool result = pigeonHasModule(vm, module);
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotBool(vm, 0, result);
}

PigeonForeignMethodFn getVariableBindMethod(const char* signature)
{
  if (strcmp(signature, "static GetVariable.beforeDefined()") == 0) return beforeDefined;
  if (strcmp(signature, "static GetVariable.afterDefined()") == 0) return afterDefined;
  if (strcmp(signature, "static GetVariable.afterAssigned()") == 0) return afterAssigned;
  if (strcmp(signature, "static GetVariable.otherSlot()") == 0) return otherSlot;
  if (strcmp(signature, "static GetVariable.otherModule()") == 0) return otherModule;
  
  if (strcmp(signature, "static Has.variable(_,_)") == 0) return hasVariable;
  if (strcmp(signature, "static Has.module(_)") == 0) return hasModule;

  return NULL;
}
