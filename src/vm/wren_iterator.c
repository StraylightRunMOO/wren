#include "wren_iterator.h"
#include "wren_vm.h"
#include "wren_value.h"
#include "wren_utils.h"
#include <string.h>

// Link an iterator into the VM's live list.
static void linkIterator(WrenVM* vm, WrenIterator* it) {
    it->prev = NULL;
    it->next = vm->liveIterators;
    if (vm->liveIterators) vm->liveIterators->prev = it;
    vm->liveIterators = it;
}

// Unlink an iterator from the VM's live list.
static void unlinkIterator(WrenVM* vm, WrenIterator* it) {
    if (it->prev) it->prev->next = it->next;
    else vm->liveIterators = it->next;
    if (it->next) it->next->prev = it->prev;
    it->prev = it->next = NULL;
}

static void listIteratorCoroutine(int argc, void** argv) {
    WrenValue listVal = *(WrenValue*)argv[0];
    ObjList* list = AS_LIST(listVal);
    for (int i = 0; i < list->elements.count; ++i) {
        if (neco_gen_yield(&list->elements.data[i]) != NECO_OK) return;
    }
}

static void mapKeyIteratorCoroutine(int argc, void** argv) {
    WrenValue mapVal = *(WrenValue*)argv[0];
    ObjMap* map = AS_MAP(mapVal);
    for (uint32_t i = 0; i < map->capacity; ++i) {
        if (IS_UNDEFINED(map->entries[i].key)) continue;
        if (neco_gen_yield(&map->entries[i].key) != NECO_OK) return;
    }
}

static void rangeIteratorCoroutine(int argc, void** argv) {
    WrenValue rangeVal = *(WrenValue*)argv[0];
    ObjRange* range = AS_RANGE(rangeVal);

    double from = range->from;
    double to = range->to;
    bool isInclusive = range->isInclusive;

    double step = (from <= to) ? 1.0 : -1.0;

    double i = from;
    for (;;) {
        if (step > 0) {
            if (isInclusive ? (i > to) : (i >= to)) break;
        } else {
            if (isInclusive ? (i < to) : (i <= to)) break;
        }
        WrenValue v = NUM_VAL(i);
        if (neco_gen_yield(&v) != NECO_OK) return;
        i += step;
    }
}

static void stringIteratorCoroutine(int argc, void** argv) {
    WrenValue strVal = *(WrenValue*)argv[0];
    ObjString* string = AS_STRING(strVal);

    uint32_t index = 0;
    while (index < string->length) {
        WrenValue v = NUM_VAL((double)(index + 1));
        if (neco_gen_yield(&v) != NECO_OK) return;

        int numBytes = wrenUtf8DecodeNumBytes((uint8_t)string->value[index]);
        if (numBytes == 0) numBytes = 1;
        index += numBytes;
    }
}

int wrenIteratorCreate(WrenVM* vm, WrenValue iterable, WrenIterator** outIter) {
    if (!outIter) return -1;
    *outIter = NULL;

    WrenIterator* it = (WrenIterator*)wrenReallocate(vm, NULL, 0, sizeof(WrenIterator));
    if (!it) return NECO_NOMEM;
    memset(it, 0, sizeof(*it));

    int r = NECO_ERROR;
    void* arg = &iterable;

    if (IS_LIST(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), listIteratorCoroutine, 1, arg);
    } else if (IS_MAP(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), mapKeyIteratorCoroutine, 1, arg);
    } else if (IS_RANGE(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), rangeIteratorCoroutine, 1, arg);
    } else if (IS_STRING(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), stringIteratorCoroutine, 1, arg);
    } else {
        r = -2;
    }

    if (r != 0) {
        wrenReallocate(vm, it, sizeof(*it), 0);
        return r;
    }

    linkIterator(vm, it);
    *outIter = it;
    return 0;
}

int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal) {
    if (!it || !it->gen || !outVal) {
        if (outVal) *outVal = NULL_VAL;
        return 0;
    }
    int r = neco_gen_next(it->gen, &it->lastValue);
    if (r == NECO_CLOSED) {
        *outVal = NULL_VAL;
        return 0;
    }
    if (r != 0) {
        *outVal = NULL_VAL;
        return 0;
    }
    *outVal = it->lastValue;
    return 1;
}

void wrenIteratorRelease(WrenVM* vm, WrenIterator* it) {
    if (!it) return;
    if (it->gen) {
        // Close the channel so the coroutine's neco_gen_yield returns NECO_CLOSED.
        neco_gen_close(it->gen);
        // Drain: call neco_gen_next to resume the coroutine so it can see the
        // close and exit. Without this, neco_start hangs waiting for the
        // suspended coroutine.
        WrenValue dummy;
        while (neco_gen_next(it->gen, &dummy) == NECO_OK) {}
        neco_gen_release(it->gen);
        it->gen = NULL;
    }
    unlinkIterator(vm, it);
    wrenReallocate(vm, it, sizeof(WrenIterator), 0);
}

void wrenIteratorReleaseAll(WrenVM* vm) {
    while (vm->liveIterators) {
        wrenIteratorRelease(vm, vm->liveIterators);
    }
}
