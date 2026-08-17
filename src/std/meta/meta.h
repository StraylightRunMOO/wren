#ifndef meta_h
#define meta_h

// Meta module - compile-time evaluation and module introspection

#include "pigeon.h"

const char* pigeonMetaSource();
PigeonForeignMethodFn pigeonMetaBindForeignMethod(PigeonVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif
