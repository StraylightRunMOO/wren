#ifndef reflection_h
#define reflection_h

// reflection - Runtime reflection and introspection for classes and objects

#include "pigeon.h"

// Method metadata for foreign classes
typedef struct {
    const char* name;
    int         arity;      // number of arguments (not counting receiver)
    bool        isGetter;
    bool        isSetter;
} PigeonMethodMeta;

// Extended foreign class binding with metadata
typedef struct {
    PigeonForeignClassMethods methods;
    const PigeonMethodMeta*   methodMeta;  // NULL-terminated array or NULL
} PigeonForeignClassWithMeta;

const char* pigeonReflectionSource();
PigeonForeignMethodFn pigeonReflectionBindForeignMethod(PigeonVM* vm,
                                                     const char* className,
                                                     bool isStatic,
                                                     const char* signature);

// Register a foreign class with method metadata (for host applications)
void pigeonBindForeignClassWithMeta(PigeonVM* vm,
                                  const char* className,
                                  PigeonForeignClassMethods* methods,
                                  const PigeonMethodMeta* methodMeta);

#endif
