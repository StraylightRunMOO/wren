#ifndef wren_random_module_h
#define wren_random_module_h

#include "wren.h"

// math/random module - High-quality PRNG suite
const char* wrenRandomModuleSource();
WrenForeignMethodFn wrenRandomModuleBindForeignMethod(WrenVM* vm,
                                                      const char* className,
                                                      bool isStatic,
                                                      const char* signature);
WrenForeignClassMethods wrenRandomModuleBindForeignClass(WrenVM* vm,
                                                         const char* className);

#endif
