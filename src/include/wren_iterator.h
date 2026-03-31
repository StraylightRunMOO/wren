#ifndef WREN_ITERATOR_H
#define WREN_ITERATOR_H

#include <stdint.h>
#include "wren.h"
#include "neco.h"   /* vendor neco.h or add as dep */

/* Define WrenValue for internal use (Value is uint64_t in wren_value.h) */
#ifndef WREN_VALUE_DEFINED
typedef uint64_t WrenValue;
#endif

typedef struct {
    neco_gen* gen;          /* owned by iterator */
    WrenValue lastValue;         /* for WrenValue return */
    _Alignas(64) char padding[56];  /* cache-line killer */
} WrenIterator;

/* Public API - call from wren_core.c primitives */
WREN_API int wrenIteratorCreate(WrenVM* vm, WrenValue iterable,
                                WrenIterator** outIter);  /* returns 0 on success */

WREN_API int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal);
/* returns 1 if value, 0 if closed (maps to your null sentinel) */

WREN_API void wrenIteratorRelease(WrenIterator* it);

#endif

