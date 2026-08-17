#ifndef pigeon_debug_h
#define pigeon_debug_h

#include "wren_value.h"
#include "wren_vm.h"

// Prints the stack trace for the current fiber.
//
// Used when a fiber throws a runtime error which is not caught.
void pigeonDebugPrintStackTrace(PigeonVM* vm);

// The "dump" functions are used for debugging Wren itself. Normal code paths
// will not call them unless one of the various DEBUG_ flags is enabled.

// Prints a representation of [value] to stdout.
void pigeonDumpValue(Value value);

// Prints a representation of the bytecode for [fn] at instruction [i].
int pigeonDumpInstruction(PigeonVM* vm, ObjFn* fn, int i);

// Prints the disassembled code for [fn] to stdout.
void pigeonDumpCode(PigeonVM* vm, ObjFn* fn);

// Prints the contents of the current stack for [fiber] to stdout.
void pigeonDumpStack(ObjFiber* fiber);

#endif
