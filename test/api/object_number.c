#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "object_number.h"

// Foreign class to test Object Numbers
typedef struct
{
  int64_t id;
} TestObject;

static void testObjectAllocate(PigeonVM* vm)
{
  TestObject* obj = (TestObject*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(TestObject));
  obj->id = 0;
}

static void testObjectGetId(PigeonVM* vm)
{
  TestObject* obj = (TestObject*)pigeonGetSlotForeign(vm, 0);
  pigeonSetSlotDouble(vm, 0, (double)obj->id);
}

PigeonForeignMethodFn objectNumberBindMethod(const char* signature)
{
  if (strcmp(signature, "TestObject.id") == 0) return testObjectGetId;
  return NULL;
}

void objectNumberBindClass(const char* className, PigeonForeignClassMethods* methods)
{
  if (strcmp(className, "TestObject") == 0)
  {
    methods->allocate = testObjectAllocate;
    methods->finalize = NULL;
  }
}

// The callback function that handles Object Numbers
static void objectNumberCallback(PigeonVM* vm, int64_t value)
{
  pigeonEnsureSlots(vm, 1);
  // For this test, just return the number as a double
  // A real application could create foreign objects, strings, or any other Wren value
  pigeonSetSlotDouble(vm, 0, (double)value);
  // The value is now in slot 0, which the compiler will read
}

int objectNumberRunTests(PigeonVM* vm)
{
  // Test that Object Numbers work
  //printf("Object Number basic test passed!\n");
  return 0;
}

PigeonVM* objectNumberCreateVM()
{
  PigeonConfiguration config;
  pigeonInitConfiguration(&config);
  config.bindForeignMethodFn = APITest_bindForeignMethod;
  config.bindForeignClassFn = APITest_bindForeignClass;
  config.objectNumberFn = objectNumberCallback;

  PigeonVM* vm = pigeonNewVM(&config);
  return vm;
}

PigeonObjectNumberFn objectNumberGetCallback()
{
  return objectNumberCallback;
}
