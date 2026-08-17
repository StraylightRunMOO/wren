#ifndef pigeon_std_io_h
#define pigeon_std_io_h

#include "wren_common.h"
#include "pigeon.h"

// Standard I/O module for Wren
// Provides console input/output operations

#if PIGEON_STD_IO

const char* pigeonStdIoSource();
PigeonForeignMethodFn pigeonStdIoBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                               const char* className,
                                               bool isStatic,
                                               const char* signature);

#endif // PIGEON_STD_IO

#endif // pigeon_std_io_h
