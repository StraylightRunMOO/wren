#include <stdio.h>
#include <string.h>

#include "slots.h"

static void noSet(PigeonVM* vm)
{
  // Do nothing.
}

static void getSlots(PigeonVM* vm)
{
  bool result = true;
  if (pigeonGetSlotBool(vm, 1) != true) result = false;

  int length;
  const char* bytes = pigeonGetSlotBytes(vm, 2, &length);
  if (length != 5) result = false;
  if (memcmp(bytes, "by\0te", length) != 0) result = false;

  if (pigeonGetSlotDouble(vm, 3) != 1.5) result = false;
  if (strcmp(pigeonGetSlotString(vm, 4), "str") != 0) result = false;

  PigeonHandle* handle = pigeonGetSlotHandle(vm, 5);

  if (result)
  {
    // Otherwise, return the value so we can tell if we captured it correctly.
    pigeonSetSlotHandle(vm, 0, handle);
  }
  else
  {
    // If anything failed, return false.
    pigeonSetSlotBool(vm, 0, false);
  }

  pigeonReleaseHandle(vm, handle);
}

static void setSlots(PigeonVM* vm)
{
  PigeonHandle* handle = pigeonGetSlotHandle(vm, 1);

  pigeonSetSlotBool(vm, 1, true);
  pigeonSetSlotBytes(vm, 2, "by\0te", 5);
  pigeonSetSlotDouble(vm, 3, 1.5);
  pigeonSetSlotString(vm, 4, "str");
  pigeonSetSlotNull(vm, 5);

  // Read the slots back to make sure they were set correctly.

  bool result = true;
  if (pigeonGetSlotBool(vm, 1) != true) result = false;

  int length;
  const char* bytes = pigeonGetSlotBytes(vm, 2, &length);
  if (length != 5) result = false;
  if (memcmp(bytes, "by\0te", length) != 0) result = false;

  if (pigeonGetSlotDouble(vm, 3) != 1.5) result = false;
  if (strcmp(pigeonGetSlotString(vm, 4), "str") != 0) result = false;

  if (pigeonGetSlotType(vm, 5) != PIGEON_TYPE_NULL) result = false;

  if (result)
  {
    // Move the value into the return position.
    pigeonSetSlotHandle(vm, 0, handle);
  }
  else
  {
    // If anything failed, return false.
    pigeonSetSlotBool(vm, 0, false);
  }

  pigeonReleaseHandle(vm, handle);
}

static void slotTypes(PigeonVM* vm)
{
  bool result =
      pigeonGetSlotType(vm, 1) == PIGEON_TYPE_BOOL &&
      pigeonGetSlotType(vm, 2) == PIGEON_TYPE_FOREIGN &&
      pigeonGetSlotType(vm, 3) == PIGEON_TYPE_LIST &&
      pigeonGetSlotType(vm, 4) == PIGEON_TYPE_MAP &&
      pigeonGetSlotType(vm, 5) == PIGEON_TYPE_NULL &&
      pigeonGetSlotType(vm, 6) == PIGEON_TYPE_NUM &&
      pigeonGetSlotType(vm, 7) == PIGEON_TYPE_STRING &&
      pigeonGetSlotType(vm, 8) == PIGEON_TYPE_UNKNOWN;

  pigeonSetSlotBool(vm, 0, result);
}

static void ensure(PigeonVM* vm)
{
  int before = pigeonGetSlotCount(vm);

  pigeonEnsureSlots(vm, 20);

  int after = pigeonGetSlotCount(vm);

  // Use the slots to make sure they're available.
  for (int i = 0; i < 20; i++)
  {
    pigeonSetSlotDouble(vm, i, i);
  }

  int sum = 0;

  for (int i = 0; i < 20; i++)
  {
    sum += (int)pigeonGetSlotDouble(vm, i);
  }

  char result[100];
  sprintf(result, "%d -> %d (%d)", before, after, sum);
  pigeonSetSlotString(vm, 0, result);
}

static void ensureOutsideForeign(PigeonVM* vm)
{
  // To test the behavior outside of a foreign method (which we're currently
  // in), create a new separate VM.
  PigeonConfiguration config;
  pigeonInitConfiguration(&config);
  PigeonVM* otherVM = pigeonNewVM(&config);

  int before = pigeonGetSlotCount(otherVM);

  pigeonEnsureSlots(otherVM, 20);

  int after = pigeonGetSlotCount(otherVM);

  // Use the slots to make sure they're available.
  for (int i = 0; i < 20; i++)
  {
    pigeonSetSlotDouble(otherVM, i, i);
  }

  int sum = 0;

  for (int i = 0; i < 20; i++)
  {
    sum += (int)pigeonGetSlotDouble(otherVM, i);
  }

  pigeonFreeVM(otherVM);

  char result[100];
  sprintf(result, "%d -> %d (%d)", before, after, sum);
  pigeonSetSlotString(vm, 0, result);
}

static void foreignClassAllocate(PigeonVM* vm)
{
  pigeonSetSlotNewForeign(vm, 0, 0, 4);
}

static void getListCount(PigeonVM* vm)
{
  pigeonSetSlotDouble(vm, 0, pigeonGetListCount(vm, 1));
}

static void getListElement(PigeonVM* vm)
{
  int index = (int)pigeonGetSlotDouble(vm, 2);
  pigeonGetListElement(vm, 1, index, 0);
}

static void getMapValue(PigeonVM* vm)
{
  pigeonGetMapValue(vm, 1, 2, 0);
}

PigeonForeignMethodFn slotsBindMethod(const char* signature)
{
  if (strcmp(signature, "static Slots.noSet") == 0) return noSet;
  if (strcmp(signature, "static Slots.getSlots(_,_,_,_,_)") == 0) return getSlots;
  if (strcmp(signature, "static Slots.setSlots(_,_,_,_,_)") == 0) return setSlots;
  if (strcmp(signature, "static Slots.slotTypes(_,_,_,_,_,_,_,_)") == 0) return slotTypes;
  if (strcmp(signature, "static Slots.ensure()") == 0) return ensure;
  if (strcmp(signature, "static Slots.ensureOutsideForeign()") == 0) return ensureOutsideForeign;
  if (strcmp(signature, "static Slots.getListCount(_)") == 0) return getListCount;
  if (strcmp(signature, "static Slots.getListElement(_,_)") == 0) return getListElement;
  if (strcmp(signature, "static Slots.getMapValue(_,_)") == 0) return getMapValue;

  return NULL;
}

void slotsBindClass(const char* className, PigeonForeignClassMethods* methods)
{
  methods->allocate = foreignClassAllocate;
}
