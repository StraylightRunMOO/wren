#ifndef pigeon_opt_meta_h
#define pigeon_opt_meta_h

#include "wren_common.h"
#include "pigeon.h"

// This module defines the Meta class and its associated methods.
#if PIGEON_OPT_META

const char* pigeonMetaSource();
PigeonForeignMethodFn pigeonMetaBindForeignMethod(PigeonVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif

#endif
