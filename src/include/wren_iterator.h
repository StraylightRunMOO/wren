#ifndef PIGEON_ITERATOR_H
#define PIGEON_ITERATOR_H

#include "pigeon.h"

/* Define PigeonValue for internal use (Value is uint64_t when NaN tagging). */
#ifndef PIGEON_VALUE_DEFINED
typedef uint64_t PigeonValue;
#endif

typedef struct PigeonIterator PigeonIterator;

PIGEON_API int pigeonIteratorCreate(PigeonVM* vm, PigeonValue iterable, PigeonIterator** outIter);
PIGEON_API int pigeonIteratorNext(PigeonIterator* restrict it, PigeonValue* restrict outVal);
PIGEON_API void pigeonIteratorRelease(PigeonVM* vm, PigeonIterator* it);

// Close the channel and cancel the producer. Safe from a GC finalizer (no VM).
PIGEON_API void pigeonIteratorShutdown(PigeonIterator* it);

// Close and release all live iterators. Call before the host event loop exits.
PIGEON_API void pigeonIteratorReleaseAll(PigeonVM* vm);

// Gray any Values the iterator holds. Called from the Generator GC hook.
PIGEON_API void pigeonIteratorGray(PigeonVM* vm, PigeonIterator* it);

#endif
