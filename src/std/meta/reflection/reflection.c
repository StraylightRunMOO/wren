#include "reflection.h"

#include <string.h>
#include <stdlib.h>

#include "wren_vm.h"
#include "reflection.wren.inc"

// Parse arity, isGetter, isSetter from a Wren method signature string.
// Wren signatures embed parameter count as underscores: "add(_,_)" = arity 2.
static void parseMethodSignature(const char* name, int nameLen,
                                 int* arity, bool* isGetter, bool* isSetter) {
    *arity = 0; *isGetter = false; *isSetter = false;
    const char* paren   = strchr(name, '(');
    const char* bracket = strchr(name, '[');
    if (!paren && !bracket) {
        // No parens: bare name is getter, bare "name=" is setter
        if (nameLen > 0 && name[nameLen - 1] == '=') { *isSetter = true; *arity = 1; }
        else { *isGetter = true; }
        return;
    }
    // Property setter: "name=(_)" — '=' immediately before '('
    if (paren && paren > name && *(paren - 1) == '=') {
        int underscores = 0;
        for (const char* p = paren + 1; *p && *p != ')'; p++)
            if (*p == '_') underscores++;
        *isSetter = true;
        *arity = underscores;
        return;
    }
    const char* open = (bracket && (!paren || bracket < paren)) ? bracket : paren;
    char close_ch = (*open == '(') ? ')' : ']';
    int underscores = 0;
    for (const char* p = open + 1; *p && *p != close_ch; p++)
        if (*p == '_') underscores++;
    const char* closePos = strchr(open, close_ch);
    // Subscript setter: "[_]=" pattern
    bool hasSuffix = closePos && *(closePos + 1) == '=';
    if (*open == '[' && hasSuffix) { *isSetter = true; *arity = underscores + 1; }
    else { *arity = underscores; }
}

static const char* methodTypeString(MethodType t) {
    switch (t) {
        case METHOD_PRIMITIVE:     return "primitive";
        case METHOD_FOREIGN:       return "foreign";
        case METHOD_BLOCK:         return "block";
        case METHOD_FUNCTION_CALL: return "function_call";
        default:                   return "none";
    }
}

// Populate a methods ObjMap with entries from `cls`, marking each as isStatic.
static void populateMethods(WrenVM* vm, ObjMap* methods, ObjClass* cls, bool isStatic) {
    SymbolTable* symbols = &vm->methodNames;
    for (int i = 0; i < cls->methods.count && i < symbols->data.count; i++) {
        Method* method = &cls->methods.data[i];
        if (method->type == METHOD_NONE) continue;

        ObjString* methodName = symbols->data.data[i];
        int arity; bool isGetter, isSetter;
        parseMethodSignature(methodName->value, methodName->length,
                             &arity, &isGetter, &isSetter);

        ObjMap* methodInfo = wrenNewMap(vm);
        wrenPushRoot(vm, (Obj*)methodInfo);

        Value vArity    = NUM_VAL(arity);
        Value vGetter   = BOOL_VAL(isGetter);
        Value vSetter   = BOOL_VAL(isSetter);
        Value vStatic   = BOOL_VAL(isStatic);
        Value vType     = OBJ_VAL(wrenNewString(vm, methodTypeString(method->type)));

        wrenMapSet(vm, methodInfo, OBJ_VAL(wrenNewString(vm, "arity")),    vArity);
        wrenMapSet(vm, methodInfo, OBJ_VAL(wrenNewString(vm, "isGetter")), vGetter);
        wrenMapSet(vm, methodInfo, OBJ_VAL(wrenNewString(vm, "isSetter")), vSetter);
        wrenMapSet(vm, methodInfo, OBJ_VAL(wrenNewString(vm, "isStatic")), vStatic);
        wrenMapSet(vm, methodInfo, OBJ_VAL(wrenNewString(vm, "type")),     vType);

        wrenMapSet(vm, methods, OBJ_VAL(methodName), OBJ_VAL(methodInfo));
        wrenPopRoot(vm);
    }
}

// Reflection_.getClass(name) — returns Map{"name":..., "methods":{...}}
static void reflectionGetClass(WrenVM* vm) {
    const char* name = wrenGetSlotString(vm, 1);

    ObjClass* foundClass = NULL;

    // Search loaded modules
    if (vm->modules && vm->modules->entries) {
        for (uint32_t i = 0; i < vm->modules->capacity; i++) {
            Value moduleVal = vm->modules->entries[i].value;
            if (IS_UNDEFINED(moduleVal) || !wrenIsObjType(moduleVal, OBJ_MODULE)) continue;
            ObjModule* module = AS_MODULE(moduleVal);
            for (int j = 0; j < module->variableNames.data.count; j++) {
                if (strcmp(module->variableNames.data.data[j]->value, name) == 0) {
                    Value v = module->variables.data[j];
                    if (IS_CLASS(v)) { foundClass = AS_CLASS(v); break; }
                }
            }
            if (foundClass) break;
        }
    }

    // Fall back to built-in classes
    if (!foundClass) {
        if      (strcmp(name, "Bool")   == 0) foundClass = vm->boolClass;
        else if (strcmp(name, "Class")  == 0) foundClass = vm->classClass;
        else if (strcmp(name, "Fiber")  == 0) foundClass = vm->fiberClass;
        else if (strcmp(name, "Fn")     == 0) foundClass = vm->fnClass;
        else if (strcmp(name, "List")   == 0) foundClass = vm->listClass;
        else if (strcmp(name, "Map")    == 0) foundClass = vm->mapClass;
        else if (strcmp(name, "Null")   == 0) foundClass = vm->nullClass;
        else if (strcmp(name, "Num")    == 0) foundClass = vm->numClass;
        else if (strcmp(name, "Object") == 0) foundClass = vm->objectClass;
        else if (strcmp(name, "Range")  == 0) foundClass = vm->rangeClass;
        else if (strcmp(name, "String") == 0) foundClass = vm->stringClass;
    }

    if (!foundClass) { wrenSetSlotNull(vm, 0); return; }

    ObjMap* result = wrenNewMap(vm);
    wrenPushRoot(vm, (Obj*)result);

    wrenMapSet(vm, result,
               OBJ_VAL(wrenNewString(vm, "name")),
               OBJ_VAL(foundClass->name));

    ObjMap* methods = wrenNewMap(vm);
    wrenPushRoot(vm, (Obj*)methods);
    wrenMapSet(vm, result, OBJ_VAL(wrenNewString(vm, "methods")), OBJ_VAL(methods));

    // Instance methods
    populateMethods(vm, methods, foundClass, false);
    // Static methods (live on the metaclass)
    populateMethods(vm, methods, foundClass->obj.classObj, true);

    wrenPopRoot(vm); // methods
    wrenPopRoot(vm); // result

    vm->apiStack[0] = OBJ_VAL(result);
}

// Reflection_.fieldsOf(instance) — returns empty list (field names not stored at runtime)
static void reflectionFieldsOf(WrenVM* vm) {
    ObjList* fields = wrenNewList(vm, 0);
    vm->apiStack[0] = OBJ_VAL(fields);
}

const char* wrenReflectionSource() {
    return reflectionModuleSource;
}

WrenForeignMethodFn wrenReflectionBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                                     const char* WREN_MAYBE_UNUSED className,
                                                     bool WREN_MAYBE_UNUSED isStatic,
                                                     const char* signature) {
    if (strcmp(className, "Reflection_") != 0) return NULL;
    if (!isStatic) return NULL;
    if (strcmp(signature, "getClass(_)") == 0)  return reflectionGetClass;
    if (strcmp(signature, "fieldsOf(_)") == 0)  return reflectionFieldsOf;
    return NULL;
}

void wrenBindForeignClassWithMeta(WrenVM* vm,
                                  const char* className,
                                  WrenForeignClassMethods* methods,
                                  const WrenMethodMeta* methodMeta) {
    (void)vm; (void)className; (void)methods; (void)methodMeta;
}
