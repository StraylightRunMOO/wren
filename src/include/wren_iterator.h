#ifndef WREN_ITERATOR_H
#define WREN_ITERATOR_H

#include "wren.h"

/* Define WrenValue for internal use (Value is uint64_t when NaN tagging). */
#ifndef WREN_VALUE_DEFINED
typedef uint64_t WrenValue;
#endif

typedef struct WrenIterator WrenIterator;

WREN_API int wrenIteratorCreate(WrenVM* vm, WrenValue iterable, WrenIterator** outIter);
WREN_API int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal);
WREN_API void wrenIteratorRelease(WrenVM* vm, WrenIterator* it);

// Close the channel and cancel the producer. Safe from a GC finalizer (no VM).
WREN_API void wrenIteratorShutdown(WrenIterator* it);

// Close and release all live iterators. Call before the host event loop exits.
WREN_API void wrenIteratorReleaseAll(WrenVM* vm);

// Gray any Values the iterator holds. Called from the Generator GC hook.
WREN_API void wrenIteratorGray(WrenVM* vm, WrenIterator* it);

#endif
