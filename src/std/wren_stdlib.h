#ifndef wren_stdlib_h
#define wren_stdlib_h

// Wren Standard Library
// Go-style hierarchical modules for common operations

#include "wren.h"
#include "io/io.h"
#include "strconv/strconv.h"
#include "time/time.h"
#include "math/math.h"
#include "math/random/random_module.h"
#include "strings/strings.h"

// Module loader for stdlib - returns source for a given module name
// Returns NULL if module not found
const char* wrenStdlibLoadModule(const char* name);

// Binds foreign methods for stdlib modules
// Called by the VM's bindForeignMethodFn callback
WrenForeignMethodFn wrenStdlibBindForeign(WrenVM* vm, const char* module, 
                                          const char* className, bool isStatic,
                                          const char* signature);

// Binds foreign classes for stdlib modules
// Called by the VM's bindForeignClassFn callback
WrenForeignClassMethods wrenStdlibBindForeignClass(WrenVM* vm, const char* module,
                                                   const char* className);

// Check if a module name is a stdlib module
int wrenStdlibHasModule(const char* name);

#endif
