#include <stdio.h>
#include <string.h>

#include "foreign_class.h"

static int finalized = 0;

static void apiFinalized(PigeonVM* vm)
{
  pigeonSetSlotDouble(vm, 0, finalized);
}

static void counterAllocate(PigeonVM* vm)
{
  double* value = (double*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(double));
  *value = 0;
}

static void counterIncrement(PigeonVM* vm)
{
  double* value = (double*)pigeonGetSlotForeign(vm, 0);
  double increment = pigeonGetSlotDouble(vm, 1);

  *value += increment;
}

static void counterValue(PigeonVM* vm)
{
  double value = *(double*)pigeonGetSlotForeign(vm, 0);
  pigeonSetSlotDouble(vm, 0, value);
}

static void pointAllocate(PigeonVM* vm)
{
  double* coordinates = (double*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(double[3]));

  // This gets called by both constructors, so sniff the slot count to see
  // which one was invoked.
  if (pigeonGetSlotCount(vm) == 1)
  {
    coordinates[0] = 0.0;
    coordinates[1] = 0.0;
    coordinates[2] = 0.0;
  }
  else
  {
    coordinates[0] = pigeonGetSlotDouble(vm, 1);
    coordinates[1] = pigeonGetSlotDouble(vm, 2);
    coordinates[2] = pigeonGetSlotDouble(vm, 3);
  }
}

static void pointTranslate(PigeonVM* vm)
{
  double* coordinates = (double*)pigeonGetSlotForeign(vm, 0);
  coordinates[0] += pigeonGetSlotDouble(vm, 1);
  coordinates[1] += pigeonGetSlotDouble(vm, 2);
  coordinates[2] += pigeonGetSlotDouble(vm, 3);
}

static void pointToString(PigeonVM* vm)
{
  double* coordinates = (double*)pigeonGetSlotForeign(vm, 0);
  char result[100];
  sprintf(result, "(%g, %g, %g)",
      coordinates[0], coordinates[1], coordinates[2]);
  pigeonSetSlotString(vm, 0, result);
}

static void resourceAllocate(PigeonVM* vm)
{
  int* value = (int*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(int));
  *value = 123;
}

static void resourceFinalize(void* data)
{
  // Make sure we get the right data back.
  int* value = (int*)data;
  if (*value != 123) exit(1);

  finalized++;
}

static void badClassAllocate(PigeonVM* vm)
{
  pigeonEnsureSlots(vm, 1);
  pigeonSetSlotString(vm, 0, "Something went wrong");
  pigeonAbortFiber(vm, 0);
}

PigeonForeignMethodFn foreignClassBindMethod(const char* signature)
{
  if (strcmp(signature, "static ForeignClass.finalized") == 0) return apiFinalized;
  if (strcmp(signature, "Counter.increment(_)") == 0) return counterIncrement;
  if (strcmp(signature, "Counter.value") == 0) return counterValue;
  if (strcmp(signature, "Point.translate(_,_,_)") == 0) return pointTranslate;
  if (strcmp(signature, "Point.toString") == 0) return pointToString;

  return NULL;
}

void foreignClassBindClass(
    const char* className, PigeonForeignClassMethods* methods)
{
  if (strcmp(className, "Counter") == 0)
  {
    methods->allocate = counterAllocate;
    return;
  }

  if (strcmp(className, "Point") == 0)
  {
    methods->allocate = pointAllocate;
    return;
  }

  if (strcmp(className, "Resource") == 0)
  {
    methods->allocate = resourceAllocate;
    methods->finalize = resourceFinalize;
    return;
  }

  if (strcmp(className, "BadClass") == 0)
  {
    methods->allocate = badClassAllocate;
    return;
  }
}
