#ifndef pigeon_core_h
#define pigeon_core_h

#include "wren_vm.h"

// This module defines the built-in classes and their primitives methods that
// are implemented directly in C code. Some languages try to implement as much
// of the core module itself in the primary language instead of in the host
// language.
//
// With Wren, we try to do as much of it in C as possible. Primitive methods
// are always faster than code written in Wren, and it minimizes startup time
// since we don't have to parse, compile, and execute Wren code.
//
// There is one limitation, though. Methods written in C cannot call Wren ones.
// They can only be the top of the callstack, and immediately return. This
// makes it difficult to have primitive methods that rely on polymorphic
// behavior. For example, `System.print` should call `toString` on its argument,
// including user-defined `toString` methods on user-defined classes.

void pigeonInitializeCore(PigeonVM* vm);

// Binds foreign methods for the core module (called for foreign methods declared
// in wren_core.wren)
PigeonForeignMethodFn pigeonCoreBindForeignMethod(const char* module, const char* className,
                                              bool isStatic, const char* signature);

// Binds foreign class allocate/finalize for the core module
PigeonForeignClassMethods pigeonCoreBindForeignClass(PigeonVM* vm, const char* className);

// GC mark callback for Generator foreign objects — grays the iterable value.
void pigeonGeneratorBlacken(PigeonVM* vm, ObjForeign* foreign);

// Drop iterator pointers from every live Generator. Call before
// pigeonIteratorReleaseAll so leftover Generator objects do not UAF.
void pigeonGeneratorDetachAll(PigeonVM* vm);

#endif
