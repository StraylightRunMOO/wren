#ifndef WREN_ITERATOR_H
#define WREN_ITERATOR_H

#include "wren.h"
#include "neco.h"

/* Define WrenValue for internal use (Value is uint64_t in wren_value.h) */
#ifndef WREN_VALUE_DEFINED
typedef uint64_t WrenValue;
#endif

typedef struct WrenIterator {
    neco_gen* gen;               /* owned, ref-counted by neco */
    WrenValue lastValue;
    struct WrenIterator* next;   /* linked list of all live iterators */
    struct WrenIterator* prev;
} WrenIterator;

WREN_API int wrenIteratorCreate(WrenVM* vm, WrenValue iterable, WrenIterator** outIter);
WREN_API int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal);  /* 1 = value, 0 = closed → NULL_VAL */
WREN_API void wrenIteratorRelease(WrenVM* vm, WrenIterator* it);

// Close and release all live iterators. Call before neco event loop exit.
WREN_API void wrenIteratorReleaseAll(WrenVM* vm);

#endif

