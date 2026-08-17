#ifndef wren_strings_h
#define wren_strings_h

#include "pigeon.h"
#include "wren_common.h"

// strings module - String utilities
const char* pigeonStringsSource();
PigeonForeignMethodFn pigeonStringsBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                 const char* PIGEON_MAYBE_UNUSED className,
                                                 bool PIGEON_MAYBE_UNUSED isStatic,
                                                 const char* PIGEON_MAYBE_UNUSED signature);

#endif
