#ifndef wren_strconv_h
#define wren_strconv_h

#include "pigeon.h"

// strconv module - String conversions
const char* pigeonStrconvSource();
PigeonForeignMethodFn pigeonStrconvBindForeignMethod(PigeonVM* vm,
                                                 const char* className,
                                                 bool isStatic,
                                                 const char* signature);

#endif
