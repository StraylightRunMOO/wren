#ifndef wren_strconv_h
#define wren_strconv_h

#include "wren.h"

// strconv module - String conversions
const char* wrenStrconvSource();
WrenForeignMethodFn wrenStrconvBindForeignMethod(WrenVM* vm,
                                                 const char* className,
                                                 bool isStatic,
                                                 const char* signature);

#endif
