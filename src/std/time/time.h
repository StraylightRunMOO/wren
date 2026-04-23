#ifndef wren_time_h
#define wren_time_h

#include "wren.h"

// time module - Time and date operations
const char* wrenTimeSource();
WrenForeignMethodFn wrenTimeBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif
