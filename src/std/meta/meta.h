#ifndef meta_h
#define meta_h

// Meta module - compile-time evaluation and module introspection

#include "wren.h"

const char* wrenMetaSource();
WrenForeignMethodFn wrenMetaBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif
