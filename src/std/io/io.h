#ifndef wren_io_h
#define wren_io_h

#include "wren.h"

// io module - Basic I/O operations
const char* wrenIoSource();
WrenForeignMethodFn wrenIoBindForeignMethod(WrenVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
WrenForeignClassMethods wrenIoBindForeignClass(WrenVM* vm,
                                               const char* className);

#endif
