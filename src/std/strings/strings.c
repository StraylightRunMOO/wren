#include "strings.h"

// strings module is pure Wren - no foreign methods needed

#include "strings.wren.inc"

const char* pigeonStringsSource() {
  return stringsModuleSource;
}

PigeonForeignMethodFn pigeonStringsBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                 const char* PIGEON_MAYBE_UNUSED className,
                                                 bool PIGEON_MAYBE_UNUSED isStatic,
                                                 const char* PIGEON_MAYBE_UNUSED signature)
{
  // Pure Wren module - no foreign methods
  return NULL;
}
