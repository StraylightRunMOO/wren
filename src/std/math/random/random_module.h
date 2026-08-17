#ifndef wren_random_module_h
#define wren_random_module_h

#include "pigeon.h"

// math/random module - High-quality PRNG suite
const char* pigeonRandomModuleSource();
PigeonForeignMethodFn pigeonRandomModuleBindForeignMethod(PigeonVM* vm,
                                                      const char* className,
                                                      bool isStatic,
                                                      const char* signature);
PigeonForeignClassMethods pigeonRandomModuleBindForeignClass(PigeonVM* vm,
                                                         const char* className);

#endif
