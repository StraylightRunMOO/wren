#ifndef WREN_ITERATOR_H
#define WREN_ITERATOR_H

#include "wren.h"
#include "neco.h"

typedef struct WrenIterator {
    neco_gen* gen;               /* owned, ref-counted by neco */
    WrenValue lastValue;
    _Alignas(64) char padding[56];
} WrenIterator;

WREN_API int wrenIteratorCreate(WrenVM* vm, WrenValue iterable, WrenIterator** outIter);
WREN_API int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal);  /* 1 = value, 0 = closed → NULL_VAL */
WREN_API void wrenIteratorRelease(WrenIterator* it);

#endif

