#ifndef wren_fs_h
#define wren_fs_h

#include "pigeon.h"

const char* pigeonFsSource();
PigeonForeignMethodFn pigeonFsBindForeignMethod(PigeonVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
PigeonForeignClassMethods pigeonFsBindForeignClass(PigeonVM* vm,
                                               const char* className);

#endif
