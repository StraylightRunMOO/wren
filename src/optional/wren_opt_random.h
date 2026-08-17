#ifndef pigeon_opt_random_h
#define pigeon_opt_random_h

#include "wren_common.h"
#include "pigeon.h"

#if PIGEON_OPT_RANDOM

const char* pigeonRandomSource();
PigeonForeignClassMethods pigeonRandomBindForeignClass(PigeonVM* vm,
                                                   const char* module,
                                                   const char* className);
PigeonForeignMethodFn pigeonRandomBindForeignMethod(PigeonVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature);

#endif

#endif
