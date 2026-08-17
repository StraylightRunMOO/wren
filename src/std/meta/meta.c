#include "meta.h"

#include <string.h>

#include "wren_vm.h"
#include "meta.wren.inc"

// Foreign method: compile source code
static void metaCompile(PigeonVM* vm)
{
  const char* source = pigeonGetSlotString(vm, 1);
  bool isExpression = pigeonGetSlotBool(vm, 2);
  bool printErrors = pigeonGetSlotBool(vm, 3);

  // Look up the module surrounding the callsite
  ObjFiber* currentFiber = vm->fiber;
  ObjFn* fn = currentFiber->frames[currentFiber->numFrames - 2].closure->fn;
  ObjString* module = fn->module->name;

  ObjClosure* closure = pigeonCompileSource(vm, module->value, source,
                                          isExpression, printErrors);
  
  // Return the result
  if (closure == NULL)
  {
    vm->apiStack[0] = NULL_VAL;
  }
  else
  {
    vm->apiStack[0] = OBJ_VAL(closure);
  }
}

// Foreign method: get module variables
static void metaGetModuleVariables(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 3);
  
  Value moduleValue = pigeonMapGet(vm, vm->modules, vm->apiStack[1]);
  if (IS_UNDEFINED(moduleValue))
  {
    vm->apiStack[0] = NULL_VAL;
    return;
  }
    
  ObjModule* module = AS_MODULE(moduleValue);
  ObjList* names = pigeonNewList(vm, module->variableNames.data.count);
  vm->apiStack[0] = OBJ_VAL(names);

  // Initialize the elements to null in case a collection happens
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = NULL_VAL;
  }
  
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = OBJ_VAL(module->variableNames.data.data[i]);
  }
}

const char* pigeonMetaSource()
{
  return metaModuleSource;
}

PigeonForeignMethodFn pigeonMetaBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                              const char* PIGEON_MAYBE_UNUSED className,
                                              bool PIGEON_MAYBE_UNUSED isStatic,
                                              const char* signature)
{
  if (strcmp(className, "meta") != 0) return NULL;
  if (!isStatic) return NULL;
  
  if (strcmp(signature, "compile_(_,_,_)") == 0)
  {
    return metaCompile;
  }
  
  if (strcmp(signature, "getModuleVariables_(_)") == 0)
  {
    return metaGetModuleVariables;
  }
  
  return NULL;
}
