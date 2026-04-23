#include "meta.h"

#include <string.h>

#include "wren_vm.h"
#include "meta.wren.inc"

// Foreign method: compile source code
static void metaCompile(WrenVM* vm)
{
  const char* source = wrenGetSlotString(vm, 1);
  bool isExpression = wrenGetSlotBool(vm, 2);
  bool printErrors = wrenGetSlotBool(vm, 3);

  // Look up the module surrounding the callsite
  ObjFiber* currentFiber = vm->fiber;
  ObjFn* fn = currentFiber->frames[currentFiber->numFrames - 2].closure->fn;
  ObjString* module = fn->module->name;

  ObjClosure* closure = wrenCompileSource(vm, module->value, source,
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
static void metaGetModuleVariables(WrenVM* vm)
{
  wrenEnsureSlots(vm, 3);
  
  Value moduleValue = wrenMapGet(vm->modules, vm->apiStack[1]);
  if (IS_UNDEFINED(moduleValue))
  {
    vm->apiStack[0] = NULL_VAL;
    return;
  }
    
  ObjModule* module = AS_MODULE(moduleValue);
  ObjList* names = wrenNewList(vm, module->variableNames.count);
  vm->apiStack[0] = OBJ_VAL(names);

  // Initialize the elements to null in case a collection happens
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = NULL_VAL;
  }
  
  for (int i = 0; i < names->elements.count; i++)
  {
    names->elements.data[i] = OBJ_VAL(module->variableNames.data[i]);
  }
}

const char* wrenMetaSource()
{
  return metaModuleSource;
}

WrenForeignMethodFn wrenMetaBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                              const char* WREN_MAYBE_UNUSED className,
                                              bool WREN_MAYBE_UNUSED isStatic,
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
