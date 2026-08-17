#include <string.h>

#include "maps.h"

static void newMap(PigeonVM* vm)
{
  pigeonSetSlotNewMap(vm, 0);
}

static void invalidInsert(PigeonVM* vm)
{
  pigeonSetSlotNewMap(vm, 0);
  
  pigeonEnsureSlots(vm, 3);
  // Foreign Class is in slot 1
  pigeonSetSlotString(vm, 2, "England");
  pigeonSetMapValue(vm, 0, 1, 2); // expect this to cause errors
}

static void insert(PigeonVM* vm)
{
  pigeonSetSlotNewMap(vm, 0);
  
  pigeonEnsureSlots(vm, 3);

  // Insert String
  pigeonSetSlotString(vm, 1, "England");
  pigeonSetSlotString(vm, 2, "London");
  pigeonSetMapValue(vm, 0, 1, 2);

  // Insert Double
  pigeonSetSlotDouble(vm, 1, 1.0);
  pigeonSetSlotDouble(vm, 2, 42.0);
  pigeonSetMapValue(vm, 0, 1, 2);

  // Insert Boolean
  pigeonSetSlotBool(vm, 1, false);
  pigeonSetSlotBool(vm, 2, true);
  pigeonSetMapValue(vm, 0, 1, 2);

  // Insert Null
  pigeonSetSlotNull(vm, 1);
  pigeonSetSlotNull(vm, 2);
  pigeonSetMapValue(vm, 0, 1, 2);

  // Insert List
  pigeonSetSlotString(vm, 1, "Empty");
  pigeonSetSlotNewList(vm, 2);
  pigeonSetMapValue(vm, 0, 1, 2);
}

static void removeKey(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 3);

  pigeonSetSlotString(vm, 2, "key");
  pigeonRemoveMapValue(vm, 1, 2, 0);
}

static void countWren(PigeonVM* vm)
{
  int count = pigeonGetMapCount(vm, 1);
  pigeonSetSlotDouble(vm, 0, count);
}

static void countAPI(PigeonVM* vm)
{
  insert(vm);
  int count = pigeonGetMapCount(vm, 0);
  pigeonSetSlotDouble(vm, 0, count);
}

static void containsWren(PigeonVM* vm)
{
  bool result = pigeonGetMapContainsKey(vm, 1, 2);
  pigeonSetSlotBool(vm, 0, result);
}


static void containsAPI(PigeonVM* vm)
{
  insert(vm);
  
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotString(vm, 1, "England");

  bool result = pigeonGetMapContainsKey(vm, 0, 1);
  pigeonSetSlotBool(vm, 0, result);
}

static void containsAPIFalse(PigeonVM* vm)
{
  insert(vm);

  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotString(vm, 1, "DefinitelyNotARealKey");

  bool result = pigeonGetMapContainsKey(vm, 0, 1);
  pigeonSetSlotBool(vm, 0, result);
}


PigeonForeignMethodFn mapsBindMethod(const char* signature)
{
  if (strcmp(signature, "static Maps.newMap()") == 0) return newMap;
  if (strcmp(signature, "static Maps.insert()") == 0) return insert;
  if (strcmp(signature, "static Maps.remove(_)") == 0) return removeKey;
  if (strcmp(signature, "static Maps.count(_)") == 0) return countWren;
  if (strcmp(signature, "static Maps.count()") == 0) return countAPI;
  if (strcmp(signature, "static Maps.contains()") == 0) return containsAPI;
  if (strcmp(signature, "static Maps.containsFalse()") == 0) return containsAPIFalse;
  if (strcmp(signature, "static Maps.contains(_,_)") == 0) return containsWren;
  if (strcmp(signature, "static Maps.invalidInsert(_)") == 0) return invalidInsert;

  return NULL;
}

void foreignAllocate(PigeonVM* vm) {
  pigeonSetSlotNewForeign(vm, 0, 0, 0);
}

void mapBindClass(
    const char* className, PigeonForeignClassMethods* methods)
{
  if (strcmp(className, "ForeignClass") == 0)
  {
    methods->allocate = foreignAllocate;
    return;
  }
}
