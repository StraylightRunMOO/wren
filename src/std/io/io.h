#ifndef wren_io_h
#define wren_io_h

#include "pigeon.h"

// io module - Basic I/O operations
const char* pigeonIoSource();
PigeonForeignMethodFn pigeonIoBindForeignMethod(PigeonVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
PigeonForeignClassMethods pigeonIoBindForeignClass(PigeonVM* vm,
                                               const char* className);

#endif
