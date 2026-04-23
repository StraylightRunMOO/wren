#ifndef wren_strings_h
#define wren_strings_h

#include "wren.h"
#include "wren_common.h"

// strings module - String utilities
const char* wrenStringsSource();
WrenForeignMethodFn wrenStringsBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                                 const char* WREN_MAYBE_UNUSED className,
                                                 bool WREN_MAYBE_UNUSED isStatic,
                                                 const char* WREN_MAYBE_UNUSED signature);

#endif
