#ifndef wren_os_h
#define wren_os_h

#include "pigeon.h"

// Call once at startup to make os.args available.
void pigeonOsSetArgs(int argc, char** argv);

const char* pigeonOsSource();
PigeonForeignMethodFn pigeonOsBindForeignMethod(PigeonVM* vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature);
PigeonForeignClassMethods pigeonOsBindForeignClass(PigeonVM* vm,
                                               const char* className);

#endif
