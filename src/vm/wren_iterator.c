#include "wren_iterator.h"
#include "wren_vm.h"
#include "wren_value.h"
#include <string.h>
#include <stdio.h>

static void listIteratorCoroutine(int argc, void **argv) {
    WrenValue listVal = *(WrenValue *)argv[0];
    ObjList *list = AS_LIST(listVal);
    for (int i = 0; i < list->elements.count; ++i) {
        neco_gen_yield(&list->elements.data[i]);
    }
}

static void mapKeyIteratorCoroutine(int argc, void **argv) {  // for .keyIterate
    WrenValue mapVal = *(WrenValue *)argv[0];
    ObjMap *map = AS_MAP(mapVal);
    for (int i = 0; i < map->capacity; ++i) {
        if (IS_UNDEFINED(map->entries[i].key)) continue;
        neco_gen_yield(&map->entries[i].key);
    }
}

static void mapIterateCoroutine(int argc, void **argv) {  // for .iterate → yield pair? or adjust tests
    // For now, match old behavior: many tests expect key on iterate? Check your test files.
    // Upstream Wren map.iterate yields the key I believe — confirm.
    WrenValue mapVal = *(WrenValue *)argv[0];
    ObjMap *map = AS_MAP(mapVal);
    for (int i = 0; i < map->capacity; ++i) {
        if (IS_UNDEFINED(map->entries[i].key)) continue;
        neco_gen_yield(&map->entries[i].key);  // or make a tuple if tests expect pair
    }
}

static void rangeIteratorCoroutine(int argc, void **argv) {
    WrenValue rangeVal = *(WrenValue *)argv[0];
    ObjRange *range = AS_RANGE(rangeVal);
    double step = (range->from <= range->to) ? 1.0 : -1.0;
    for (double i = range->from; 
         (step > 0 ? i <= range->to : i >= range->to); 
         i += step) {
        WrenValue v = NUM_VAL(i);
        neco_gen_yield(&v);
    }
}

// Add string later

int wrenIteratorCreate(WrenVM *vm, WrenValue iterable, WrenIterator **outIter) {
    if (!outIter) return -1;
    *outIter = NULL;

    WrenIterator *it = (WrenIterator *)wrenReallocate(vm, NULL, 0, sizeof(WrenIterator));
    if (!it) return -1;
    memset(it, 0, sizeof(*it));

    int r;
    void *arg = &iterable;

    if (IS_LIST(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), listIteratorCoroutine, 1, arg);
    } else if (IS_MAP(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), mapKeyIteratorCoroutine, 1, arg);  // or mapIterateCoroutine depending on test
    } else if (IS_RANGE(iterable)) {
        r = neco_gen_start(&it->gen, sizeof(WrenValue), rangeIteratorCoroutine, 1, arg);
    } else if (IS_STRING(iterable)) {
        // TODO: char or codepoint yields
        r = -3;
    } else {
        r = -2;  // foreign
    }

    if (r != 0) {
        wrenReallocate(vm, it, sizeof(*it), 0);
        return r;
    }

    *outIter = it;
    return 0;
}


int wrenIteratorNext(WrenIterator* restrict it, WrenValue* restrict outVal) {
    if (!it || !it->gen || !outVal) return 0;

    int r = neco_gen_next(it->gen, &it->lastValue);
    if (r == NECO_CLOSED) {
        *outVal = NULL_VAL;   /* your falsy sentinel */
        return 0;
    }
    if (r != 0) {
        *outVal = NULL_VAL;
        return 0;
    }

    *outVal = it->lastValue;
    return 1;
}

void wrenIteratorRelease(WrenIterator* it) {
    if (!it) return;
    if (it->gen) {
        neco_gen_release(it->gen);
        it->gen = NULL;
    }
    /* Wren GC will free the wrapper via wrenReallocate */
}

