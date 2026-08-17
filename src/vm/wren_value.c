#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "pigeon.h"
#include "wren_value.h"
#include "wren_vm.h"
#include "wren_core.h"

#if PIGEON_DEBUG_TRACE_MEMORY
  #include "wren_debug.h"
#endif

// TODO: Tune these.
// The initial (and minimum) capacity of a non-empty list or map object.
#define MIN_CAPACITY 16

// The rate at which a collection's capacity grows when the size exceeds the
// current capacity. The new capacity will be determined by *multiplying* the
// old capacity by this. Growing geometrically is necessary to ensure that
// adding to a collection has O(1) amortized complexity.
#define GROW_FACTOR 2

// The maximum percentage of map entries that can be filled before the map is
// grown. A lower load takes more memory but reduces collisions which makes
// lookup faster.
#define MAP_LOAD_PERCENT 75

// The number of call frames initially allocated when a fiber is created. Making
// this smaller makes fibers use less memory (at first) but spends more time
// reallocating when the call stack grows.
#define INITIAL_CALL_FRAMES 4

DEFINE_BUFFER(Value, Value);
DEFINE_BUFFER(Method, Method);

static void initObj(PigeonVM* vm, Obj* obj, ObjType type, ObjClass* classObj)
{
  obj->type = type;
  obj->isDark = false;
  obj->classObj = classObj;
  obj->next = vm->first;
  vm->first = obj;
}

ObjClass* pigeonNewSingleClass(PigeonVM* vm, int numFields, ObjString* name)
{
  ObjClass* classObj = ALLOCATE(vm, ObjClass);
  initObj(vm, &classObj->obj, OBJ_CLASS, NULL);
  classObj->superclass = NULL;
  classObj->numFields = numFields;
  classObj->name = name;
  classObj->attributes = NULL_VAL;

  pigeonPushRoot(vm, (Obj*)classObj);
  pigeonMethodBufferInit(&classObj->methods);
  pigeonPopRoot(vm);

  return classObj;
}

void pigeonBindSuperclass(PigeonVM* vm, ObjClass* subclass, ObjClass* superclass)
{
  ASSERT(superclass != NULL, "Must have superclass.");

  subclass->superclass = superclass;

  // Include the superclass in the total number of fields.
  if (subclass->numFields != -1)
  {
    subclass->numFields += superclass->numFields;
  }
  else
  {
    ASSERT(superclass->numFields == 0,
           "A foreign class cannot inherit from a class with fields.");
  }

  // Inherit methods from its superclass.
  for (int i = 0; i < superclass->methods.count; i++)
  {
    pigeonBindMethod(vm, subclass, i, superclass->methods.data[i]);
  }
}

ObjClass* pigeonNewClass(PigeonVM* vm, ObjClass* superclass, int numFields,
                       ObjString* name)
{
  // Create the metaclass.
  Value metaclassName = pigeonStringFormat(vm, "@ metaclass", OBJ_VAL(name));
  pigeonPushRoot(vm, AS_OBJ(metaclassName));

  ObjClass* metaclass = pigeonNewSingleClass(vm, 0, AS_STRING(metaclassName));
  metaclass->obj.classObj = vm->classClass;

  pigeonPopRoot(vm);

  // Make sure the metaclass isn't collected when we allocate the class.
  pigeonPushRoot(vm, (Obj*)metaclass);

  // Metaclasses always inherit Class and do not parallel the non-metaclass
  // hierarchy.
  pigeonBindSuperclass(vm, metaclass, vm->classClass);

  ObjClass* classObj = pigeonNewSingleClass(vm, numFields, name);

  // Make sure the class isn't collected while the inherited methods are being
  // bound.
  pigeonPushRoot(vm, (Obj*)classObj);

  classObj->obj.classObj = metaclass;
  pigeonBindSuperclass(vm, classObj, superclass);

  pigeonPopRoot(vm);
  pigeonPopRoot(vm);

  return classObj;
}

void pigeonBindMethod(PigeonVM* vm, ObjClass* classObj, int symbol, Method method)
{
  // Make sure the buffer is big enough to contain the symbol's index.
  if (symbol >= classObj->methods.count)
  {
    Method noMethod;
    noMethod.type = METHOD_NONE;
    pigeonMethodBufferFill(vm, &classObj->methods, noMethod,
                         symbol - classObj->methods.count + 1);
  }

  classObj->methods.data[symbol] = method;
}

ObjClosure* pigeonNewClosure(PigeonVM* vm, ObjFn* fn)
{
  ObjClosure* closure = ALLOCATE_FLEX(vm, ObjClosure,
                                      ObjUpvalue*, fn->numUpvalues);
  initObj(vm, &closure->obj, OBJ_CLOSURE, vm->fnClass);

  closure->fn = fn;

  // Clear the upvalue array. We need to do this in case a GC is triggered
  // after the closure is created but before the upvalue array is populated.
  for (int i = 0; i < fn->numUpvalues; i++) closure->upvalues[i] = NULL;

  return closure;
}

ObjFiber* pigeonNewFiber(PigeonVM* vm, ObjClosure* closure)
{
  // Allocate the arrays before the fiber in case it triggers a GC.
  CallFrame* frames = ALLOCATE_ARRAY(vm, CallFrame, INITIAL_CALL_FRAMES);
  
  // Add one slot for the unused implicit receiver slot that the compiler
  // assumes all functions have.
  int stackCapacity = closure == NULL
      ? 1
      : pigeonPowerOf2Ceil(closure->fn->maxSlots + 1);
  Value* stack = ALLOCATE_ARRAY(vm, Value, stackCapacity);
  
  ObjFiber* fiber = ALLOCATE(vm, ObjFiber);
  initObj(vm, &fiber->obj, OBJ_FIBER, vm->fiberClass);

  fiber->stack = stack;
  fiber->stackTop = fiber->stack;
  fiber->stackCapacity = stackCapacity;

  fiber->frames = frames;
  fiber->frameCapacity = INITIAL_CALL_FRAMES;
  fiber->numFrames = 0;

  fiber->openUpvalues = NULL;
  fiber->caller = NULL;
  fiber->error = NULL_VAL;
  fiber->state = FIBER_OTHER;
  
  if (closure != NULL)
  {
    // Initialize the first call frame.
    pigeonAppendCallFrame(vm, fiber, closure, fiber->stack);

    // The first slot always holds the closure.
    fiber->stackTop[0] = OBJ_VAL(closure);
    fiber->stackTop++;
  }
  
  return fiber;
}

void pigeonEnsureStack(PigeonVM* vm, ObjFiber* fiber, int needed)
{
  if (fiber->stackCapacity >= needed) return;
  
  int capacity = pigeonPowerOf2Ceil(needed);
  
  Value* oldStack = fiber->stack;
  fiber->stack = (Value*)pigeonReallocate(vm, fiber->stack,
                                        sizeof(Value) * fiber->stackCapacity,
                                        sizeof(Value) * capacity);
  fiber->stackCapacity = capacity;
  
  // If the reallocation moves the stack, then we need to recalculate every
  // pointer that points into the old stack to into the same relative distance
  // in the new stack. We have to be a little careful about how these are
  // calculated because pointer subtraction is only well-defined within a
  // single array, hence the slightly redundant-looking arithmetic below.
  if (fiber->stack != oldStack)
  {
    // Top of the stack.
    if (vm->apiStack >= oldStack && vm->apiStack <= fiber->stackTop)
    {
      vm->apiStack = fiber->stack + (vm->apiStack - oldStack);
    }
    
    // Stack pointer for each call frame.
    for (int i = 0; i < fiber->numFrames; i++)
    {
      CallFrame* frame = &fiber->frames[i];
      frame->stackStart = fiber->stack + (frame->stackStart - oldStack);
    }
    
    // Open upvalues.
    for (ObjUpvalue* upvalue = fiber->openUpvalues;
         upvalue != NULL;
         upvalue = upvalue->next)
    {
      upvalue->value = fiber->stack + (upvalue->value - oldStack);
    }
    
    fiber->stackTop = fiber->stack + (fiber->stackTop - oldStack);
  }
}

ObjForeign* pigeonNewForeign(PigeonVM* vm, ObjClass* classObj, size_t size)
{
  ObjForeign* object = ALLOCATE_FLEX(vm, ObjForeign, uint8_t, size);
  initObj(vm, &object->obj, OBJ_FOREIGN, classObj);

  // Zero out the bytes.
  memset(object->data, 0, size);
  return object;
}

ObjFn* pigeonNewFunction(PigeonVM* vm, ObjModule* module, int maxSlots)
{
  FnDebug* debug = ALLOCATE(vm, FnDebug);
  debug->name = NULL;
  pigeonIntBufferInit(&debug->sourceLines);

  ObjFn* fn = ALLOCATE(vm, ObjFn);
  initObj(vm, &fn->obj, OBJ_FN, vm->fnClass);
  
  pigeonValueBufferInit(&fn->constants);
  pigeonByteBufferInit(&fn->code);
  fn->module = module;
  fn->maxSlots = maxSlots;
  fn->numUpvalues = 0;
  fn->arity = 0;
  fn->debug = debug;
  fn->ics = NULL;
  fn->icsCount = 0;
  
  return fn;
}

void pigeonFunctionBindName(PigeonVM* vm, ObjFn* fn, const char* name, int length)
{
  fn->debug->name = ALLOCATE_ARRAY(vm, char, length + 1);
  memcpy(fn->debug->name, name, length);
  fn->debug->name[length] = '\0';
}

Value pigeonNewInstance(PigeonVM* vm, ObjClass* classObj)
{
  ObjInstance* instance = ALLOCATE_FLEX(vm, ObjInstance,
                                        Value, classObj->numFields);
  initObj(vm, &instance->obj, OBJ_INSTANCE, classObj);

  // Initialize fields to null.
  for (int i = 0; i < classObj->numFields; i++)
  {
    instance->fields[i] = NULL_VAL;
  }

  return OBJ_VAL(instance);
}

ObjList* pigeonNewList(PigeonVM* vm, uint32_t numElements)
{
  // Allocate this before the list object in case it triggers a GC which would
  // free the list.
  Value* elements = NULL;
  if (numElements > 0)
  {
    elements = ALLOCATE_ARRAY(vm, Value, numElements);
  }

  ObjList* list = ALLOCATE(vm, ObjList);
  initObj(vm, &list->obj, OBJ_LIST, vm->listClass);
  list->elements.capacity = numElements;
  list->elements.count = numElements;
  list->elements.data = elements;
  return list;
}

void pigeonListInsert(PigeonVM* vm, ObjList* list, Value value, uint32_t index)
{
  if (IS_OBJ(value)) pigeonPushRoot(vm, AS_OBJ(value));

  // Add a slot at the end of the list.
  pigeonValueBufferWrite(vm, &list->elements, NULL_VAL);

  if (IS_OBJ(value)) pigeonPopRoot(vm);

  // Shift the existing elements down.
  for (uint32_t i = list->elements.count - 1; i > index; i--)
  {
    list->elements.data[i] = list->elements.data[i - 1];
  }

  // Store the new element.
  list->elements.data[index] = value;
}

int pigeonListIndexOf(PigeonVM* PIGEON_MAYBE_UNUSED vm, ObjList* list, Value value)
{
  int count = list->elements.count;
  for (int i = 0; i < count; i++)
  {
    Value item = list->elements.data[i];
    if(pigeonValuesEqual(item, value)) {
      return i;
    }
  }
  return -1;
}

Value pigeonListRemoveAt(PigeonVM* vm, ObjList* list, uint32_t index)
{
  Value removed = list->elements.data[index];

  if (IS_OBJ(removed)) pigeonPushRoot(vm, AS_OBJ(removed));

  // Shift items up.
  for (int i = index; i < list->elements.count - 1; i++)
  {
    list->elements.data[i] = list->elements.data[i + 1];
  }

  // If we have too much excess capacity, shrink it.
  if (list->elements.capacity / GROW_FACTOR >= list->elements.count)
  {
    list->elements.data = (Value*)pigeonReallocate(vm, list->elements.data,
        sizeof(Value) * list->elements.capacity,
        sizeof(Value) * (list->elements.capacity / GROW_FACTOR));
    list->elements.capacity /= GROW_FACTOR;
  }

  if (IS_OBJ(removed)) pigeonPopRoot(vm);

  list->elements.count--;
  return removed;
}

ObjMap* pigeonNewMap(PigeonVM* vm)
{
  ObjMap* map = ALLOCATE(vm, ObjMap);
  initObj(vm, &map->obj, OBJ_MAP, vm->mapClass);
  map->capacity = 0;
  map->count = 0;
  map->entries = NULL;
  return map;
}

static inline uint32_t hashBits(uint64_t hash, uint32_t seed)
{
  // From v8's ComputeLongHash() which in turn cites:
  // Thomas Wang, Integer Hash Functions.
  // http://www.concentric.net/~Ttwang/tech/inthash.htm
  hash ^= (uint64_t)seed;
  hash = ~hash + (hash << 18);  // hash = (hash << 18) - hash - 1;
  hash = hash ^ (hash >> 31);
  hash = hash * 21;  // hash = (hash + (hash << 2)) + (hash << 4);
  hash = hash ^ (hash >> 11);
  hash = hash + (hash << 6);
  hash = hash ^ (hash >> 22);
  return (uint32_t)(hash & 0x3fffffff);
}

static inline uint32_t hashNumber(double num, uint32_t seed)
{
  return hashBits(pigeonDoubleToBits(num), seed);
}

static uint32_t hashObject(Obj* object, uint32_t seed)
{
  switch (object->type)
  {
    case OBJ_CLASS:
      return hashObject((Obj*)((ObjClass*)object)->name, seed);

      // Allow bare (non-closure) functions so that we can use a map to find
      // existing constants in a function's constant table. This is only used
      // internally. Since user code never sees a non-closure function, they
      // cannot use them as map keys.
    case OBJ_FN:
    {
      ObjFn* fn = (ObjFn*)object;
      return hashNumber(fn->arity, seed) ^ hashNumber(fn->code.count, seed);
    }

    case OBJ_RANGE:
    {
      ObjRange* range = (ObjRange*)object;
      return hashNumber(range->from, seed) ^ hashNumber(range->to, seed);
    }

    case OBJ_STRING:
      return ((ObjString*)object)->hash;

    default:
      ASSERT(false, "Only immutable objects can be hashed.");
      return 0;
  }
}

static uint32_t hashValue(Value value, uint32_t seed)
{
#if PIGEON_NAN_TAGGING
  if (IS_OBJ(value)) return hashObject(AS_OBJ(value), seed);

  return hashBits(value, seed);
#else
  switch (value.type)
  {
    case VAL_FALSE: return 0;
    case VAL_NULL:  return 1;
    case VAL_NUM:   return hashNumber(AS_NUM(value), seed);
    case VAL_TRUE:  return 2;
    case VAL_OBJ:   return hashObject(AS_OBJ(value), seed);
    default:        UNREACHABLE();
  }

  return 0;
#endif
}

// Looks for an entry with [key] in an array of [capacity] [entries].
//
// If found, sets [result] to point to it and returns `true`. Otherwise,
// returns `false` and points [result] to the entry where the key/value pair
// should be inserted.
static bool findEntry(MapEntry* entries, uint32_t capacity, Value key,
                      uint32_t seed, MapEntry** result)
{
  // If there is no entry array (an empty map), we definitely won't find it.
  if (capacity == 0) return false;

  // Figure out where to insert it in the table. Use open addressing and
  // basic linear probing.
  uint32_t startIndex = hashValue(key, seed) % capacity;
  uint32_t index = startIndex;
  
  // If we pass a tombstone and don't end up finding the key, its entry will
  // be re-used for the insert.
  MapEntry* tombstone = NULL;
  
  // Walk the probe sequence until we've tried every slot.
  do
  {
    MapEntry* entry = &entries[index];
    
    if (IS_UNDEFINED(entry->key))
    {
      // If we found an empty slot, the key is not in the table. If we found a
      // slot that contains a deleted key, we have to keep looking.
      if (IS_FALSE(entry->value))
      {
        // We found an empty slot, so we've reached the end of the probe
        // sequence without finding the key. If we passed a tombstone, then
        // that's where we should insert the item, otherwise, put it here at
        // the end of the sequence.
        *result = tombstone != NULL ? tombstone : entry;
        return false;
      }
      else
      {
        // We found a tombstone. We need to keep looking in case the key is
        // after it, but we'll use this entry as the insertion point if the
        // key ends up not being found.
        if (tombstone == NULL) tombstone = entry;
      }
    }
    else if (pigeonValuesEqual(entry->key, key))
    {
      // We found the key.
      *result = entry;
      return true;
    }
    
    // Try the next slot.
    index = (index + 1) % capacity;
  }
  while (index != startIndex);
  
  // If we get here, the table is full of tombstones. Return the first one we
  // found.
  ASSERT(tombstone != NULL, "Map should have tombstones or empty entries.");
  *result = tombstone;
  return false;
}

// Inserts [key] and [value] in the array of [entries] with the given
// [capacity].
//
// Returns `true` if this is the first time [key] was added to the map.
static bool insertEntry(MapEntry* entries, uint32_t capacity,
                        Value key, Value value, uint32_t seed)
{
  ASSERT(entries != NULL, "Should ensure capacity before inserting.");

  MapEntry* entry;
  if (findEntry(entries, capacity, key, seed, &entry))
  {
    // Already present, so just replace the value.
    entry->value = value;
    return false;
  }
  else
  {
    entry->key = key;
    entry->value = value;
    return true;
  }
}

// Updates [map]'s entry array to [capacity].
static void resizeMap(PigeonVM* vm, ObjMap* map, uint32_t capacity)
{
  uint32_t seed = vm->hashSeed;

  // Create the new empty hash table.
  MapEntry* entries = ALLOCATE_ARRAY(vm, MapEntry, capacity);
  for (uint32_t i = 0; i < capacity; i++)
  {
    entries[i].key = UNDEFINED_VAL;
    entries[i].value = FALSE_VAL;
  }

  // Re-add the existing entries.
  if (map->capacity > 0)
  {
    for (uint32_t i = 0; i < map->capacity; i++)
    {
      MapEntry* entry = &map->entries[i];
      
      // Don't copy empty entries or tombstones.
      if (IS_UNDEFINED(entry->key)) continue;

      insertEntry(entries, capacity, entry->key, entry->value, seed);
    }
  }

  // Replace the array.
  DEALLOCATE(vm, map->entries);
  map->entries = entries;
  map->capacity = capacity;
}

Value pigeonMapGet(PigeonVM* vm, ObjMap* map, Value key)
{
  MapEntry* entry;
  if (findEntry(map->entries, map->capacity, key, vm->hashSeed, &entry))
    return entry->value;

  return UNDEFINED_VAL;
}

void pigeonMapSet(PigeonVM* vm, ObjMap* map, Value key, Value value)
{
  // If the map is getting too full, make room first.
  if (map->count + 1 > map->capacity * MAP_LOAD_PERCENT / 100)
  {
    // Figure out the new hash table size.
    uint32_t capacity = map->capacity * GROW_FACTOR;
    if (capacity < MIN_CAPACITY) capacity = MIN_CAPACITY;

    resizeMap(vm, map, capacity);
  }

  if (insertEntry(map->entries, map->capacity, key, value, vm->hashSeed))
  {
    // A new key was added.
    map->count++;
  }
}

void pigeonMapClear(PigeonVM* vm, ObjMap* map)
{
  DEALLOCATE(vm, map->entries);
  map->entries = NULL;
  map->capacity = 0;
  map->count = 0;
}

Value pigeonMapRemoveKey(PigeonVM* vm, ObjMap* map, Value key)
{
  MapEntry* entry;
  if (!findEntry(map->entries, map->capacity, key, vm->hashSeed, &entry))
    return NULL_VAL;

  // Remove the entry from the map. Set this value to true, which marks it as a
  // deleted slot. When searching for a key, we will stop on empty slots, but
  // continue past deleted slots.
  Value value = entry->value;
  entry->key = UNDEFINED_VAL;
  entry->value = TRUE_VAL;

  if (IS_OBJ(value)) pigeonPushRoot(vm, AS_OBJ(value));

  map->count--;

  if (map->count == 0)
  {
    // Removed the last item, so free the array.
    pigeonMapClear(vm, map);
  }
  else if (map->capacity > MIN_CAPACITY &&
           map->count < map->capacity / GROW_FACTOR * MAP_LOAD_PERCENT / 100)
  {
    uint32_t capacity = map->capacity / GROW_FACTOR;
    if (capacity < MIN_CAPACITY) capacity = MIN_CAPACITY;

    // The map is getting empty, so shrink the entry array back down.
    // TODO: Should we do this less aggressively than we grow?
    resizeMap(vm, map, capacity);
  }

  if (IS_OBJ(value)) pigeonPopRoot(vm);
  return value;
}

ObjModule* pigeonNewModule(PigeonVM* vm, ObjString* name)
{
  ObjModule* module = ALLOCATE(vm, ObjModule);

  // Modules are never used as first-class objects, so don't need a class.
  initObj(vm, (Obj*)module, OBJ_MODULE, NULL);

  pigeonPushRoot(vm, (Obj*)module);

  pigeonSymbolTableInit(&module->variableNames);
  pigeonValueBufferInit(&module->variables);

  module->name = name;

  pigeonPopRoot(vm);
  return module;
}

Value pigeonNewRange(PigeonVM* vm, double from, double to, bool isInclusive)
{
  ObjRange* range = ALLOCATE(vm, ObjRange);
  initObj(vm, &range->obj, OBJ_RANGE, vm->rangeClass);
  range->from = from;
  range->to = to;
  range->isInclusive = isInclusive;

  return OBJ_VAL(range);
}

// Creates a new string object with a null-terminated buffer large enough to
// hold a string of [length] but does not fill in the bytes.
//
// The caller is expected to fill in the buffer and then calculate the string's
// hash.
static ObjString* allocateString(PigeonVM* vm, size_t length)
{
  ObjString* string = ALLOCATE_FLEX(vm, ObjString, char, length + 1);
  initObj(vm, &string->obj, OBJ_STRING, vm->stringClass);
  string->length = (int)length;
  string->value[length] = '\0';

  return string;
}

static void hashString(ObjString* string, uint32_t seed)
{
  // FNV-1a hash, seeded per-VM for hash flooding resistance.
  uint32_t hash = seed ^ 2166136261u;

  for (uint32_t i = 0; i < string->length; i++)
  {
    hash ^= string->value[i];
    hash *= 16777619;
  }

  string->hash = hash;
}

Value pigeonNewString(PigeonVM* vm, const char* text)
{
  return pigeonNewStringLength(vm, text, strlen(text));
}

Value pigeonNewStringLength(PigeonVM* vm, const char* text, size_t length)
{
  // Allow NULL if the string is empty since byte buffers don't allocate any
  // characters for a zero-length string.
  ASSERT(length == 0 || text != NULL, "Unexpected NULL string.");
  
  ObjString* string = allocateString(vm, length);
  
  // Copy the string (if given one).
  if (length > 0 && text != NULL) memcpy(string->value, text, length);
  
  hashString(string, vm->hashSeed);
  return OBJ_VAL(string);
}


Value pigeonNewStringFromRange(PigeonVM* vm, ObjString* source, int start,
                             uint32_t count, int step)
{
  uint8_t* from = (uint8_t*)source->value;
  int length = 0;
  for (uint32_t i = 0; i < count; i++)
  {
    length += pigeonUtf8DecodeNumBytes(from[start + i * step]);
  }

  ObjString* result = allocateString(vm, length);
  result->value[length] = '\0';

  uint8_t* to = (uint8_t*)result->value;
  for (uint32_t i = 0; i < count; i++)
  {
    int index = start + i * step;
    int codePoint = pigeonUtf8Decode(from + index, source->length - index);

    if (codePoint != -1)
    {
      to += pigeonUtf8Encode(codePoint, to);
    }
  }

  hashString(result, vm->hashSeed);
  return OBJ_VAL(result);
}

Value pigeonNumToString(PigeonVM* vm, double value)
{
  // Edge case: If the value is NaN or infinity, different versions of libc
  // produce different outputs (some will format it signed and some won't). To
  // get reliable output, handle it ourselves.
  if (isnan(value)) return CONST_STRING(vm, "nan");
  if (isinf(value))
  {
    if (value > 0.0)
    {
      return CONST_STRING(vm, "infinity");
    }
    else
    {
      return CONST_STRING(vm, "-infinity");
    }
  }

  // This is large enough to hold any double converted to a string using
  // "%.14g". Example:
  //
  //     -1.12345678901234e-1022
  //
  // So we have:
  //
  // + 1 char for sign
  // + 1 char for digit
  // + 1 char for "."
  // + 14 chars for decimal digits
  // + 1 char for "e"
  // + 1 char for "-" or "+"
  // + 4 chars for exponent
  // + 1 char for "\0"
  // = 24
  char buffer[24];
  int length = sprintf(buffer, "%.14g", value);
  return pigeonNewStringLength(vm, buffer, length);
}

Value pigeonStringFromCodePoint(PigeonVM* vm, int value)
{
  int length = pigeonUtf8EncodeNumBytes(value);
  ASSERT(length != 0, "Value out of range.");

  ObjString* string = allocateString(vm, length);

  pigeonUtf8Encode(value, (uint8_t*)string->value);
  hashString(string, vm->hashSeed);

  return OBJ_VAL(string);
}

Value pigeonStringFromByte(PigeonVM *vm, uint8_t value)
{
  int length = 1;
  ObjString* string = allocateString(vm, length);
  string->value[0] = value;
  hashString(string, vm->hashSeed);
  return OBJ_VAL(string);
}

Value pigeonStringFormat(PigeonVM* vm, const char* format, ...)
{
  va_list argList;

  // Calculate the length of the result string. Do this up front so we can
  // create the final string with a single allocation.
  va_start(argList, format);
  size_t totalLength = 0;
  for (const char* c = format; *c != '\0'; c++)
  {
    switch (*c)
    {
      case '$':
        totalLength += strlen(va_arg(argList, const char*));
        break;

      case '@':
        totalLength += AS_STRING(va_arg(argList, Value))->length;
        break;

      default:
        // Any other character is interpreted literally.
        totalLength++;
    }
  }
  va_end(argList);

  // Concatenate the string.
  ObjString* result = allocateString(vm, totalLength);

  va_start(argList, format);
  char* start = result->value;
  for (const char* c = format; *c != '\0'; c++)
  {
    switch (*c)
    {
      case '$':
      {
        const char* string = va_arg(argList, const char*);
        size_t length = strlen(string);
        memcpy(start, string, length);
        start += length;
        break;
      }

      case '@':
      {
        ObjString* string = AS_STRING(va_arg(argList, Value));
        memcpy(start, string->value, string->length);
        start += string->length;
        break;
      }

      default:
        // Any other character is interpreted literally.
        *start++ = *c;
    }
  }
  va_end(argList);

  hashString(result, vm->hashSeed);

  return OBJ_VAL(result);
}

Value pigeonStringCodePointAt(PigeonVM* vm, ObjString* string, uint32_t index)
{
  ASSERT(index < string->length, "Index out of bounds.");

  int codePoint = pigeonUtf8Decode((uint8_t*)string->value + index,
                                 string->length - index);
  if (codePoint == -1)
  {
    // If it isn't a valid UTF-8 sequence, treat it as a single raw byte.
    char bytes[2];
    bytes[0] = string->value[index];
    bytes[1] = '\0';
    return pigeonNewStringLength(vm, bytes, 1);
  }

  return pigeonStringFromCodePoint(vm, codePoint);
}

// Uses the Boyer-Moore-Horspool string matching algorithm.
uint32_t pigeonStringFind(ObjString* haystack, ObjString* needle, uint32_t start)
{
  // Edge case: An empty needle is always found.
  if (needle->length == 0) return start;

  // If the needle goes past the haystack it won't be found.
  if (start + needle->length > haystack->length) return UINT32_MAX;

  // If the startIndex is too far it also won't be found.
  if (start >= haystack->length) return UINT32_MAX;

  // Pre-calculate the shift table. For each character (8-bit value), we
  // determine how far the search window can be advanced if that character is
  // the last character in the haystack where we are searching for the needle
  // and the needle doesn't match there.
  uint32_t shift[UINT8_MAX];
  uint32_t needleEnd = needle->length - 1;

  // By default, we assume the character is not the needle at all. In that case
  // case, if a match fails on that character, we can advance one whole needle
  // width since.
  for (uint32_t index = 0; index < UINT8_MAX; index++)
  {
    shift[index] = needle->length;
  }

  // Then, for every character in the needle, determine how far it is from the
  // end. If a match fails on that character, we can advance the window such
  // that it the last character in it lines up with the last place we could
  // find it in the needle.
  for (uint32_t index = 0; index < needleEnd; index++)
  {
    char c = needle->value[index];
    shift[(uint8_t)c] = needleEnd - index;
  }

  // Slide the needle across the haystack, looking for the first match or
  // stopping if the needle goes off the end.
  char lastChar = needle->value[needleEnd];
  uint32_t range = haystack->length - needle->length;

  for (uint32_t index = start; index <= range; )
  {
    // Compare the last character in the haystack's window to the last character
    // in the needle. If it matches, see if the whole needle matches.
    char c = haystack->value[index + needleEnd];
    if (lastChar == c &&
        memcmp(haystack->value + index, needle->value, needleEnd) == 0)
    {
      // Found a match.
      return index;
    }

    // Otherwise, slide the needle forward.
    index += shift[(uint8_t)c];
  }

  // Not found.
  return UINT32_MAX;
}

ObjUpvalue* pigeonNewUpvalue(PigeonVM* vm, Value* value)
{
  ObjUpvalue* upvalue = ALLOCATE(vm, ObjUpvalue);

  // Upvalues are never used as first-class objects, so don't need a class.
  initObj(vm, &upvalue->obj, OBJ_UPVALUE, NULL);

  upvalue->value = value;
  upvalue->closed = NULL_VAL;
  upvalue->next = NULL;
  return upvalue;
}

void pigeonGrayObj(PigeonVM* vm, Obj* obj)
{
  if (obj == NULL) return;

  // Stop if the object is already darkened so we don't get stuck in a cycle.
  if (obj->isDark) return;

  // It's been reached.
  obj->isDark = true;

  // Add it to the gray list so it can be recursively explored for
  // more marks later.
  if (vm->grayCount >= vm->grayCapacity)
  {
    vm->grayCapacity = vm->grayCount * 2;
    vm->gray = (Obj**)vm->config.reallocateFn(vm->gray,
                                              vm->grayCapacity * sizeof(Obj*),
                                              vm->config.userData);
  }

  vm->gray[vm->grayCount++] = obj;
}

void pigeonGrayValue(PigeonVM* vm, Value value)
{
  if (!IS_OBJ(value)) return;
  pigeonGrayObj(vm, AS_OBJ(value));
}

void pigeonGrayBuffer(PigeonVM* vm, ValueBuffer* buffer)
{
  for (int i = 0; i < buffer->count; i++)
  {
    pigeonGrayValue(vm, buffer->data[i]);
  }
}

static void blackenClass(PigeonVM* vm, ObjClass* classObj)
{
  // The metaclass.
  pigeonGrayObj(vm, (Obj*)classObj->obj.classObj);

  // The superclass.
  pigeonGrayObj(vm, (Obj*)classObj->superclass);

  // Method function objects.
  for (int i = 0; i < classObj->methods.count; i++)
  {
    if (classObj->methods.data[i].type == METHOD_BLOCK)
    {
      pigeonGrayObj(vm, (Obj*)classObj->methods.data[i].as.closure);
    }
  }

  pigeonGrayObj(vm, (Obj*)classObj->name);

  if(!IS_NULL(classObj->attributes)) pigeonGrayObj(vm, AS_OBJ(classObj->attributes));

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClass);
  vm->bytesAllocated += classObj->methods.capacity * sizeof(Method);
}

static void blackenClosure(PigeonVM* vm, ObjClosure* closure)
{
  // Mark the function.
  pigeonGrayObj(vm, (Obj*)closure->fn);

  // Mark the upvalues.
  for (int i = 0; i < closure->fn->numUpvalues; i++)
  {
    pigeonGrayObj(vm, (Obj*)closure->upvalues[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjClosure);
  vm->bytesAllocated += sizeof(ObjUpvalue*) * closure->fn->numUpvalues;
}

static void blackenFiber(PigeonVM* vm, ObjFiber* fiber)
{
  // Stack functions.
  for (int i = 0; i < fiber->numFrames; i++)
  {
    pigeonGrayObj(vm, (Obj*)fiber->frames[i].closure);
  }

  // Stack variables.
  for (Value* slot = fiber->stack; slot < fiber->stackTop; slot++)
  {
    pigeonGrayValue(vm, *slot);
  }

  // Open upvalues.
  ObjUpvalue* upvalue = fiber->openUpvalues;
  while (upvalue != NULL)
  {
    pigeonGrayObj(vm, (Obj*)upvalue);
    upvalue = upvalue->next;
  }

  // The caller.
  pigeonGrayObj(vm, (Obj*)fiber->caller);
  pigeonGrayValue(vm, fiber->error);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFiber);
  vm->bytesAllocated += fiber->frameCapacity * sizeof(CallFrame);
  vm->bytesAllocated += fiber->stackCapacity * sizeof(Value);
}

static void blackenFn(PigeonVM* vm, ObjFn* fn)
{
  // Mark the constants.
  pigeonGrayBuffer(vm, &fn->constants);

  // Mark the module it belongs to, in case it's been unloaded.
  pigeonGrayObj(vm, (Obj*)fn->module);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjFn);
  vm->bytesAllocated += sizeof(uint8_t) * fn->code.capacity;
  vm->bytesAllocated += sizeof(Value) * fn->constants.capacity;
  
  // The debug line number buffer.
  vm->bytesAllocated += sizeof(int) * fn->code.capacity;
  vm->bytesAllocated += sizeof(InlineCache) * fn->icsCount;
  // TODO: What about the function name?

  if (fn->ics != NULL)
  {
    for (int i = 0; i < fn->icsCount; i++)
    {
      pigeonGrayObj(vm, (Obj*)fn->ics[i].klass);
    }
  }
}

static void blackenForeign(PigeonVM* vm, ObjForeign* foreign)
{
  // Generator objects store the iterable Value that must be kept alive
  // while the producer coroutine yields copies of its elements.
  if (vm->generatorClass != NULL &&
      foreign->obj.classObj == vm->generatorClass) {
    pigeonGeneratorBlacken(vm, foreign);
  }
}

static void blackenInstance(PigeonVM* vm, ObjInstance* instance)
{
  pigeonGrayObj(vm, (Obj*)instance->obj.classObj);

  // Mark the fields.
  for (int i = 0; i < instance->obj.classObj->numFields; i++)
  {
    pigeonGrayValue(vm, instance->fields[i]);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjInstance);
  vm->bytesAllocated += sizeof(Value) * instance->obj.classObj->numFields;
}

static void blackenList(PigeonVM* vm, ObjList* list)
{
  // Mark the elements.
  pigeonGrayBuffer(vm, &list->elements);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjList);
  vm->bytesAllocated += sizeof(Value) * list->elements.capacity;
}

static void blackenMap(PigeonVM* vm, ObjMap* map)
{
  // Mark the entries.
  for (uint32_t i = 0; i < map->capacity; i++)
  {
    MapEntry* entry = &map->entries[i];
    if (IS_UNDEFINED(entry->key)) continue;

    pigeonGrayValue(vm, entry->key);
    pigeonGrayValue(vm, entry->value);
  }

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjMap);
  vm->bytesAllocated += sizeof(MapEntry) * map->capacity;
}

static void blackenModule(PigeonVM* vm, ObjModule* module)
{
  // Top-level variables.
  for (int i = 0; i < module->variables.count; i++)
  {
    pigeonGrayValue(vm, module->variables.data[i]);
  }

  pigeonBlackenSymbolTable(vm, &module->variableNames);

  pigeonGrayObj(vm, (Obj*)module->name);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjModule);
}

static void blackenRange(PigeonVM* vm, ObjRange* PIGEON_MAYBE_UNUSED range)
{
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjRange);
}

static void blackenString(PigeonVM* vm, ObjString* string)
{
  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjString) + string->length + 1;
}

static void blackenUpvalue(PigeonVM* vm, ObjUpvalue* upvalue)
{
  // Mark the closed-over object (in case it is closed).
  pigeonGrayValue(vm, upvalue->closed);

  // Keep track of how much memory is still in use.
  vm->bytesAllocated += sizeof(ObjUpvalue);
}

static void blackenObject(PigeonVM* vm, Obj* obj)
{
#if PIGEON_DEBUG_TRACE_MEMORY
  printf("mark ");
  pigeonDumpValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  // Traverse the object's fields.
  switch (obj->type)
  {
    case OBJ_CLASS:    blackenClass(   vm, (ObjClass*)   obj); break;
    case OBJ_CLOSURE:  blackenClosure( vm, (ObjClosure*) obj); break;
    case OBJ_FIBER:    blackenFiber(   vm, (ObjFiber*)   obj); break;
    case OBJ_FN:       blackenFn(      vm, (ObjFn*)      obj); break;
    case OBJ_FOREIGN:  blackenForeign( vm, (ObjForeign*) obj); break;
    case OBJ_INSTANCE: blackenInstance(vm, (ObjInstance*)obj); break;
    case OBJ_LIST:     blackenList(    vm, (ObjList*)    obj); break;
    case OBJ_MAP:      blackenMap(     vm, (ObjMap*)     obj); break;
    case OBJ_MODULE:   blackenModule(  vm, (ObjModule*)  obj); break;
    case OBJ_RANGE:    blackenRange(   vm, (ObjRange*)   obj); break;
    case OBJ_STRING:   blackenString(  vm, (ObjString*)  obj); break;
    case OBJ_UPVALUE:  blackenUpvalue( vm, (ObjUpvalue*) obj); break;
  }
}

void pigeonBlackenObjects(PigeonVM* vm)
{
  while (vm->grayCount > 0)
  {
    // Pop an item from the gray stack.
    Obj* obj = vm->gray[--vm->grayCount];
    blackenObject(vm, obj);
  }
}

void pigeonFreeObj(PigeonVM* vm, Obj* obj)
{
#if PIGEON_DEBUG_TRACE_MEMORY
  printf("free ");
  pigeonDumpValue(OBJ_VAL(obj));
  printf(" @ %p\n", obj);
#endif

  switch (obj->type)
  {
    case OBJ_CLASS:
      pigeonMethodBufferClear(vm, &((ObjClass*)obj)->methods);
      break;

    case OBJ_FIBER:
    {
      ObjFiber* fiber = (ObjFiber*)obj;
      DEALLOCATE(vm, fiber->frames);
      DEALLOCATE(vm, fiber->stack);
      break;
    }
      
    case OBJ_FN:
    {
      ObjFn* fn = (ObjFn*)obj;
      pigeonValueBufferClear(vm, &fn->constants);
      pigeonByteBufferClear(vm, &fn->code);
      pigeonIntBufferClear(vm, &fn->debug->sourceLines);
      DEALLOCATE(vm, fn->debug->name);
      DEALLOCATE(vm, fn->debug);
      DEALLOCATE(vm, fn->ics);
      break;
    }

    case OBJ_FOREIGN:
      pigeonFinalizeForeign(vm, (ObjForeign*)obj);
      break;

    case OBJ_LIST:
      pigeonValueBufferClear(vm, &((ObjList*)obj)->elements);
      break;

    case OBJ_MAP:
      DEALLOCATE(vm, ((ObjMap*)obj)->entries);
      break;

    case OBJ_MODULE:
      pigeonSymbolTableClear(vm, &((ObjModule*)obj)->variableNames);
      pigeonValueBufferClear(vm, &((ObjModule*)obj)->variables);
      break;

    case OBJ_CLOSURE:
    case OBJ_INSTANCE:
    case OBJ_RANGE:
    case OBJ_STRING:
    case OBJ_UPVALUE:
      break;
  }

  DEALLOCATE(vm, obj);
}

ObjClass* pigeonGetClass(PigeonVM* vm, Value value)
{
  return pigeonGetClassInline(vm, value);
}

bool pigeonValuesEqual(Value a, Value b)
{
  if (pigeonValuesSame(a, b)) return true;

  // If we get here, it's only possible for two heap-allocated immutable objects
  // to be equal.
  if (!IS_OBJ(a) || !IS_OBJ(b)) return false;

  Obj* aObj = AS_OBJ(a);
  Obj* bObj = AS_OBJ(b);

  // Must be the same type.
  if (aObj->type != bObj->type) return false;

  switch (aObj->type)
  {
    case OBJ_RANGE:
    {
      ObjRange* aRange = (ObjRange*)aObj;
      ObjRange* bRange = (ObjRange*)bObj;
      return aRange->from == bRange->from &&
             aRange->to == bRange->to &&
             aRange->isInclusive == bRange->isInclusive;
    }

    case OBJ_STRING:
    {
      ObjString* aString = (ObjString*)aObj;
      ObjString* bString = (ObjString*)bObj;
      return aString->hash == bString->hash &&
      pigeonStringEqualsCString(aString, bString->value, bString->length);
    }

    default:
      // All other types are only equal if they are same, which they aren't if
      // we get here.
      return false;
  }
}
