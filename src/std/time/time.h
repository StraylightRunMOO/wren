#ifndef wren_time_h
#define wren_time_h

#include "pigeon.h"

// time module - Time and date operations
const char* pigeonTimeSource();
PigeonForeignMethodFn pigeonTimeBindForeignMethod(PigeonVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif
