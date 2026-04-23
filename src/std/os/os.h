#ifndef wren_os_h
#define wren_os_h

#include "wren.h"

// Call once at startup to make os.args available.
void wrenOsSetArgs(int argc, char** argv);

const char* wrenOsSource();
WrenForeignMethodFn wrenOsBindForeignMethod(WrenVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
WrenForeignClassMethods wrenOsBindForeignClass(WrenVM* vm,
                                               const char* className);

#endif
