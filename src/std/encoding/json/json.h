#ifndef wren_json_h
#define wren_json_h

#include "wren.h"

const char* wrenJsonSource();
WrenForeignMethodFn wrenJsonBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);
WrenForeignClassMethods wrenJsonBindForeignClass(WrenVM* vm,
                                                 const char* className);

#endif
