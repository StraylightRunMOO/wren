#ifndef wren_std_io_h
#define wren_std_io_h

#include "wren_common.h"
#include "wren.h"

// Standard I/O module for Wren
// Provides console input/output operations

#if WREN_STD_IO

const char* wrenStdIoSource();
WrenForeignMethodFn wrenStdIoBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* className,
                                               bool isStatic,
                                               const char* signature);

#endif // WREN_STD_IO

#endif // wren_std_io_h
