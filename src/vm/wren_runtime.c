// Single implementation TU for Memento + Suspenders.
// Include order is required: Memento first, then Suspenders.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define MEMENTO_IMPLEMENTATION
#include "memento.h"

#define SUSPENDERS_IMPLEMENTATION
#include "suspenders.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Wren's host hook is realloc-shaped (no oldSize on free). Memento needs the
// exact size passed to alloc. A 16-byte header holds a magic + that size.
// Allocations are rounded up to the size-class block so free always matches
// the carved block.

#define PIGEON_MEM_MAGIC 0x4D454D314E525257ull /* "WRN1MEM" */
#define PIGEON_MEM_PREFIX 16

typedef struct {
  uint64_t magic;
  uint64_t total;
} PigeonMemHeader;

static size_t pigeonMemPack(size_t user)
{
  size_t total = PIGEON_MEM_PREFIX + user;
  if (total <= MEMENTO_MAX_SIZE_CLASS)
  {
    size_t sc = memento_size_class_for(total);
    return memento_size_class_to_size(sc);
  }
  return total;
}

static PigeonMemHeader* pigeonMemHeader(void* payload)
{
  return (PigeonMemHeader*)((char*)payload - PIGEON_MEM_PREFIX);
}

void* pigeonDefaultReallocate(void* ptr, size_t newSize, void* userData)
{
  (void)userData;

  if (!memento_init())
  {
    if (newSize == 0)
    {
      free(ptr);
      return NULL;
    }
    return realloc(ptr, newSize);
  }

  memento_thread_heap_t* heap = memento_thread_heap_get();
  if (heap == NULL)
  {
    if (newSize == 0)
    {
      free(ptr);
      return NULL;
    }
    return realloc(ptr, newSize);
  }

  if (newSize == 0)
  {
    if (ptr == NULL) return NULL;
    PigeonMemHeader* h = pigeonMemHeader(ptr);
    if (h->magic != PIGEON_MEM_MAGIC)
    {
      // Double-free or a non-Memento pointer. Do not touch the freelist.
      return NULL;
    }
    h->magic = 0;
    memento_thread_heap_free(heap, h, (size_t)h->total);
    return NULL;
  }

  size_t newTotal = pigeonMemPack(newSize);
  if (ptr == NULL)
  {
    PigeonMemHeader* h = (PigeonMemHeader*)memento_thread_heap_alloc(heap, newTotal);
    if (h == NULL) return NULL;
    h->magic = PIGEON_MEM_MAGIC;
    h->total = newTotal;
    return (char*)h + PIGEON_MEM_PREFIX;
  }

  PigeonMemHeader* old = pigeonMemHeader(ptr);
  if (old->magic != PIGEON_MEM_MAGIC)
  {
    return NULL;
  }

  size_t oldTotal = (size_t)old->total;
  if (newTotal == oldTotal) return ptr;

  PigeonMemHeader* grown = (PigeonMemHeader*)memento_thread_heap_realloc(
      heap, old, oldTotal, newTotal);
  if (grown == NULL) return NULL;
  grown->magic = PIGEON_MEM_MAGIC;
  grown->total = newTotal;
  return (char*)grown + PIGEON_MEM_PREFIX;
}
