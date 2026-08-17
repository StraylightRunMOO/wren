#ifndef pigeon_stdlib_h
#define pigeon_stdlib_h

// Wren Standard Library
// Go-style hierarchical modules for common operations

#include "pigeon.h"
#include "io/io.h"
#include "fs/fs.h"
#include "os/os.h"
#include "strconv/strconv.h"
#include "time/time.h"
#include "math/math.h"
#include "math/random/random_module.h"
#include "strings/strings.h"
#include "meta/meta.h"
#include "meta/reflection/reflection.h"
#include "encoding/json/json.h"

// Module loader for stdlib - returns source for a given module name
// Returns NULL if module not found
const char* pigeonStdlibLoadModule(const char* name);

// Binds foreign methods for stdlib modules
// Called by the VM's bindForeignMethodFn callback
PigeonForeignMethodFn pigeonStdlibBindForeign(PigeonVM* vm, const char* module, 
                                          const char* className, bool isStatic,
                                          const char* signature);

// Binds foreign classes for stdlib modules
// Called by the VM's bindForeignClassFn callback
PigeonForeignClassMethods pigeonStdlibBindForeignClass(PigeonVM* vm, const char* module,
                                                   const char* className);

// Check if a module name is a stdlib module
int pigeonStdlibHasModule(const char* name);

// Get the default export name for a module (e.g., "Math" for "math")
// Returns NULL if module not found or has no default export
const char* pigeonStdlibGetDefaultExport(const char* name);

// Get the list of exports for a module (for wildcard imports)
// Returns NULL if module not found
const char** pigeonStdlibGetExports(const char* name);

#endif
