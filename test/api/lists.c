#include <string.h>

#include "lists.h"

static void newList(PigeonVM* vm)
{
  pigeonSetSlotNewList(vm, 0);
}

// Helper function to store a double in a slot then insert it into the list at
// slot zero.
static void insertNumber(PigeonVM* vm, int index, double value)
{
  pigeonSetSlotDouble(vm, 1, value);
  pigeonInsertInList(vm, 0, index, 1);
}

// Helper function to append a double in a slot then insert it into the list at
// slot zero.
static void appendNumber(PigeonVM* vm, double value)
{
  pigeonSetSlotDouble(vm, 1, value);
  pigeonInsertInList(vm, 0, -1, 1);
}

static void insert(PigeonVM* vm)
{
  pigeonSetSlotNewList(vm, 0);

  pigeonEnsureSlots(vm, 2);

  // Appending.
  insertNumber(vm, 0, 1.0);
  insertNumber(vm, 1, 2.0);
  insertNumber(vm, 2, 3.0);

  // Inserting.
  insertNumber(vm, 0, 4.0);
  insertNumber(vm, 1, 5.0);
  insertNumber(vm, 2, 6.0);

  // Negative indexes.
  insertNumber(vm, -1, 7.0);
  insertNumber(vm, -2, 8.0);
  insertNumber(vm, -3, 9.0);
}

static void get(PigeonVM* vm)
{
  int listSlot = 1;
  int index = (int)pigeonGetSlotDouble(vm, 2);

  pigeonGetListElement(vm, listSlot, index, 0);
}

static void set(PigeonVM* vm)
{
  pigeonSetSlotNewList(vm, 0);

  pigeonEnsureSlots(vm, 2);

  appendNumber(vm, 1.0);
  appendNumber(vm, 2.0);
  appendNumber(vm, 3.0);
  appendNumber(vm, 4.0);
  
  //list[2] = 33
  pigeonSetSlotDouble(vm, 1, 33);
  pigeonSetListElement(vm, 0, 2, 1);

  //list[-1] = 44
  pigeonSetSlotDouble(vm, 1, 44);
  pigeonSetListElement(vm, 0, -1, 1);
}

PigeonForeignMethodFn listsBindMethod(const char* signature)
{
  if (strcmp(signature, "static Lists.newList()") == 0) return newList;
  if (strcmp(signature, "static Lists.insert()") == 0) return insert;
  if (strcmp(signature, "static Lists.set()") == 0) return set;
  if (strcmp(signature, "static Lists.get(_,_)") == 0) return get;

  return NULL;
}
