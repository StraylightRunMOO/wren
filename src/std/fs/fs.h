#ifndef wren_fs_h
#define wren_fs_h

#include "wren.h"

const char* wrenFsSource();
WrenForeignMethodFn wrenFsBindForeignMethod(WrenVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
WrenForeignClassMethods wrenFsBindForeignClass(WrenVM* vm,
                                               const char* className);

#endif
