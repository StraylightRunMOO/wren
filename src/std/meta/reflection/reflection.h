#ifndef reflection_h
#define reflection_h

// reflection - Runtime reflection and introspection for classes and objects

#include "wren.h"

// Method metadata for foreign classes
typedef struct {
    const char* name;
    int         arity;      // number of arguments (not counting receiver)
    bool        isGetter;
    bool        isSetter;
} WrenMethodMeta;

// Extended foreign class binding with metadata
typedef struct {
    WrenForeignClassMethods methods;
    const WrenMethodMeta*   methodMeta;  // NULL-terminated array or NULL
} WrenForeignClassWithMeta;

const char* wrenReflectionSource();
WrenForeignMethodFn wrenReflectionBindForeignMethod(WrenVM* vm,
                                                     const char* className,
                                                     bool isStatic,
                                                     const char* signature);

// Register a foreign class with method metadata (for host applications)
void wrenBindForeignClassWithMeta(WrenVM* vm,
                                  const char* className,
                                  WrenForeignClassMethods* methods,
                                  const WrenMethodMeta* methodMeta);

#endif
