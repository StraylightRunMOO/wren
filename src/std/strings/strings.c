#include "strings.h"

// strings module is pure Wren - no foreign methods needed

#include "strings.wren.inc"

const char* wrenStringsSource() {
  return stringsModuleSource;
}

WrenForeignMethodFn wrenStringsBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                                 const char* WREN_MAYBE_UNUSED className,
                                                 bool WREN_MAYBE_UNUSED isStatic,
                                                 const char* WREN_MAYBE_UNUSED signature)
{
  // Pure Wren module - no foreign methods
  return NULL;
}
