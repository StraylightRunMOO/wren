#ifndef wren_json_h
#define wren_json_h

#include "pigeon.h"

const char* pigeonJsonSource();
PigeonForeignMethodFn pigeonJsonBindForeignMethod(PigeonVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);
PigeonForeignClassMethods pigeonJsonBindForeignClass(PigeonVM* vm,
                                                 const char* className);

#endif
