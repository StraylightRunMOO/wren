#ifndef wren_utils_h
#define wren_utils_h

#include "wren.h"
#include "wren_common.h"

// Reusable data structures and other utility functions.

// Forward declare this here to break a cycle between wren_utils.h and
// wren_value.h.
typedef struct sObjString ObjString;

// We need buffers of a few different types. To avoid lots of casting between
// void* and back, we'll use the preprocessor as a poor man's generics and let
// it generate a few type-specific ones.
#define DECLARE_BUFFER(name, type)                                             \
    typedef struct                                                             \
    {                                                                          \
      type* data;                                                              \
      int count;                                                               \
      int capacity;                                                            \
    } name##Buffer;                                                            \
    void wren##name##BufferInit(name##Buffer* buffer);                         \
    void wren##name##BufferClear(WrenVM* vm, name##Buffer* buffer);            \
    void wren##name##BufferFill(WrenVM* vm, name##Buffer* buffer, type data,   \
                                int count);                                    \
    void wren##name##BufferWrite(WrenVM* vm, name##Buffer* buffer, type data)

// This should be used once for each type instantiation, somewhere in a .c file.
#define DEFINE_BUFFER(name, type)                                              \
    void wren##name##BufferInit(name##Buffer* buffer)                          \
    {                                                                          \
      buffer->data = NULL;                                                     \
      buffer->capacity = 0;                                                    \
      buffer->count = 0;                                                       \
    }                                                                          \
                                                                               \
    void wren##name##BufferClear(WrenVM* vm, name##Buffer* buffer)             \
    {                                                                          \
      wrenReallocate(vm, buffer->data, 0, 0);                                  \
      wren##name##BufferInit(buffer);                                          \
    }                                                                          \
                                                                               \
    void wren##name##BufferFill(WrenVM* vm, name##Buffer* buffer, type data,   \
                                int count)                                     \
    {                                                                          \
      if (buffer->capacity < buffer->count + count)                            \
      {                                                                        \
        int capacity = wrenPowerOf2Ceil(buffer->count + count);                \
        buffer->data = (type*)wrenReallocate(vm, buffer->data,                 \
            buffer->capacity * sizeof(type), capacity * sizeof(type));         \
        buffer->capacity = capacity;                                           \
      }                                                                        \
                                                                               \
      for (int i = 0; i < count; i++)                                          \
      {                                                                        \
        buffer->data[buffer->count++] = data;                                  \
      }                                                                        \
    }                                                                          \
                                                                               \
    void wren##name##BufferWrite(WrenVM* vm, name##Buffer* buffer, type data)  \
    {                                                                          \
      wren##name##BufferFill(vm, buffer, data, 1);                             \
    }

DECLARE_BUFFER(Byte, uint8_t);
DECLARE_BUFFER(Int, int);
DECLARE_BUFFER(String, ObjString*);

// ---------------------------------------------------------------------------
// Swizz.h hash table instantiation for symbol lookup acceleration.
// Maps null-terminated C strings → int (symbol index).
// ---------------------------------------------------------------------------
#include <string.h>

static inline char* swizz_sym_dup(const char* k)
{
  if (!k) return NULL;
  size_t len = strlen(k);
  char* copy = (char*)malloc(len + 1);
  if (copy) memcpy(copy, k, len + 1);
  return copy;
}

#define SWIZZ_NAME          sym
#define SWIZZ_KEY_TYPE      const char*
#define SWIZZ_VALUE_TYPE    int
#define SWIZZ_HASH(k)       hash_string(k)
#define SWIZZ_EQ(a, b)      (strcmp((a), (b)) == 0)
#define SWIZZ_DUP_KEY(k)    swizz_sym_dup(k)
#define SWIZZ_FREE_KEY(k)   free((void*)(k))
#include "swizz.h"

// The symbol table is an array (for index-based dispatch) plus a hash map
// (for fast name → index lookup).
typedef struct
{
  StringBuffer data;
  sym_table    index;
} SymbolTable;

// Initializes the symbol table.
void wrenSymbolTableInit(SymbolTable* symbols);

// Frees all dynamically allocated memory used by the symbol table, but not the
// SymbolTable itself.
void wrenSymbolTableClear(WrenVM* vm, SymbolTable* symbols);

// Adds name to the symbol table. Returns the index of it in the table.
int wrenSymbolTableAdd(WrenVM* vm, SymbolTable* symbols,
                       const char* name, size_t length);

// Adds name to the symbol table. Returns the index of it in the table. Will
// use an existing symbol if already present.
int wrenSymbolTableEnsure(WrenVM* vm, SymbolTable* symbols,
                          const char* name, size_t length);

// Looks up name in the symbol table. Returns its index if found or -1 if not.
int wrenSymbolTableFind(const SymbolTable* symbols,
                        const char* name, size_t length);

void wrenBlackenSymbolTable(WrenVM* vm, SymbolTable* symbolTable);

// Returns the number of bytes needed to encode [value] in UTF-8.
//
// Returns 0 if [value] is too large to encode.
int wrenUtf8EncodeNumBytes(int value);

// Encodes value as a series of bytes in [bytes], which is assumed to be large
// enough to hold the encoded result.
//
// Returns the number of written bytes.
int wrenUtf8Encode(int value, uint8_t* bytes);

// Decodes the UTF-8 sequence starting at [bytes] (which has max [length]),
// returning the code point.
//
// Returns -1 if the bytes are not a valid UTF-8 sequence.
int wrenUtf8Decode(const uint8_t* bytes, uint32_t length);

// Returns the number of bytes in the UTF-8 sequence starting with [byte].
//
// If the character at that index is not the beginning of a UTF-8 sequence,
// returns 0.
int wrenUtf8DecodeNumBytes(uint8_t byte);

// Returns the smallest power of two that is equal to or greater than [n].
int wrenPowerOf2Ceil(int n);

// Validates that [value] is within `[0, count)`. Also allows
// negative indices which map backwards from the end. Returns the valid positive
// index value. If invalid, returns `UINT32_MAX`.
uint32_t wrenValidateIndex(uint32_t count, int64_t value);

#endif
