#ifndef wren_math_module_h
#define wren_math_module_h

#include "wren.h"

// math module - Mathematical functions
const char* wrenMathModuleSource();
WrenForeignMethodFn wrenMathModuleBindForeignMethod(WrenVM* vm,
                                                    const char* className,
                                                    bool isStatic,
                                                    const char* signature);

#endif
