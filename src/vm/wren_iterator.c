#include "wren_iterator.h"
#include "wren_vm.h"
#include "wren_value.h"
#include "wren_utils.h"

#include <string.h>

typedef enum {
    ITER_LIST,
    ITER_MAP,
    ITER_RANGE,
    ITER_STRING
} IterKind;

struct PigeonIterator {
    IterKind kind;
    PigeonValue iterable;
    PigeonValue lastValue;
    // LIST/STRING: next element/byte index.
    // MAP: next slot to scan. RANGE: unused (see rangePos).
    uint32_t index;
    double rangePos;
    PigeonIterator* next;
    PigeonIterator* prev;
};

static void linkIterator(PigeonVM* vm, PigeonIterator* it)
{
    it->prev = NULL;
    it->next = vm->liveIterators;
    if (vm->liveIterators) vm->liveIterators->prev = it;
    vm->liveIterators = it;
}

static void unlinkIterator(PigeonVM* vm, PigeonIterator* it)
{
    if (it->prev) it->prev->next = it->next;
    else vm->liveIterators = it->next;
    if (it->next) it->next->prev = it->prev;
    it->prev = it->next = NULL;
}

int pigeonIteratorCreate(PigeonVM* vm, PigeonValue iterable, PigeonIterator** outIter)
{
    if (!outIter) return -1;
    *outIter = NULL;

    IterKind kind;
    if (IS_LIST(iterable)) kind = ITER_LIST;
    else if (IS_MAP(iterable)) kind = ITER_MAP;
    else if (IS_RANGE(iterable)) kind = ITER_RANGE;
    else if (IS_STRING(iterable)) kind = ITER_STRING;
    else return -2;

    PigeonIterator* it = (PigeonIterator*)pigeonReallocate(vm, NULL, 0, sizeof(PigeonIterator));
    if (!it) return -1;
    memset(it, 0, sizeof(*it));
    it->kind = kind;
    it->iterable = iterable;
    it->lastValue = NULL_VAL;
    it->index = 0;
    if (kind == ITER_RANGE) it->rangePos = AS_RANGE(iterable)->from;

    linkIterator(vm, it);
    *outIter = it;
    return 0;
}

int pigeonIteratorNext(PigeonIterator* restrict it, PigeonValue* restrict outVal)
{
    if (!it || !outVal)
    {
        if (outVal) *outVal = NULL_VAL;
        return 0;
    }

    switch (it->kind)
    {
        case ITER_LIST:
        {
            ObjList* list = AS_LIST(it->iterable);
            if (it->index >= (uint32_t)list->elements.count)
            {
                *outVal = NULL_VAL;
                return 0;
            }
            it->lastValue = list->elements.data[it->index++];
            *outVal = it->lastValue;
            return 1;
        }
        case ITER_MAP:
        {
            ObjMap* map = AS_MAP(it->iterable);
            while (it->index < map->capacity)
            {
                uint32_t i = it->index++;
                if (IS_UNDEFINED(map->entries[i].key)) continue;
                it->lastValue = map->entries[i].key;
                *outVal = it->lastValue;
                return 1;
            }
            *outVal = NULL_VAL;
            return 0;
        }
        case ITER_RANGE:
        {
            ObjRange* range = AS_RANGE(it->iterable);
            double to = range->to;
            bool isInclusive = range->isInclusive;
            double step = (range->from <= to) ? 1.0 : -1.0;
            double i = it->rangePos;
            if (step > 0)
            {
                if (isInclusive ? (i > to) : (i >= to))
                {
                    *outVal = NULL_VAL;
                    return 0;
                }
            }
            else
            {
                if (isInclusive ? (i < to) : (i <= to))
                {
                    *outVal = NULL_VAL;
                    return 0;
                }
            }
            it->lastValue = NUM_VAL(i);
            it->rangePos = i + step;
            *outVal = it->lastValue;
            return 1;
        }
        case ITER_STRING:
        {
            ObjString* string = AS_STRING(it->iterable);
            if (it->index >= string->length)
            {
                *outVal = NULL_VAL;
                return 0;
            }
            it->lastValue = NUM_VAL((double)(it->index + 1));
            int numBytes = pigeonUtf8DecodeNumBytes((uint8_t)string->value[it->index]);
            if (numBytes == 0) numBytes = 1;
            it->index += (uint32_t)numBytes;
            *outVal = it->lastValue;
            return 1;
        }
    }

    *outVal = NULL_VAL;
    return 0;
}

void pigeonIteratorShutdown(PigeonIterator* it)
{
    (void)it;
}

void pigeonIteratorRelease(PigeonVM* vm, PigeonIterator* it)
{
    if (!it) return;
    unlinkIterator(vm, it);
    pigeonReallocate(vm, it, sizeof(PigeonIterator), 0);
}

void pigeonIteratorGray(PigeonVM* vm, PigeonIterator* it)
{
    if (!it) return;
    pigeonGrayValue(vm, it->lastValue);
    pigeonGrayValue(vm, it->iterable);
}

void pigeonIteratorReleaseAll(PigeonVM* vm)
{
    while (vm->liveIterators)
    {
        pigeonIteratorRelease(vm, vm->liveIterators);
    }
}
