#ifndef wren_math_module_h
#define wren_math_module_h

#include "pigeon.h"

// math module - Mathematical functions
const char* pigeonMathModuleSource();
PigeonForeignMethodFn pigeonMathModuleBindForeignMethod(PigeonVM* vm,
                                                    const char* className,
                                                    bool isStatic,
                                                    const char* signature);

#endif
