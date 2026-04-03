#ifndef wren_core_h
#define wren_core_h

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

void wrenInitializeCore(WrenVM* vm);

// Binds foreign methods for the core module (called for foreign methods declared
// in wren_core.wren)
WrenForeignMethodFn wrenCoreBindForeignMethod(const char* module, const char* className,
                                              bool isStatic, const char* signature);

#endif
