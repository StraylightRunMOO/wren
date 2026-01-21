#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "object_number.h"

// Foreign class to test Object Numbers
typedef struct
{
  int64_t id;
} TestObject;

static void testObjectAllocate(WrenVM* vm)
{
  TestObject* obj = (TestObject*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(TestObject));
  obj->id = 0;
}

static void testObjectGetId(WrenVM* vm)
{
  TestObject* obj = (TestObject*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, (double)obj->id);
}

WrenForeignMethodFn objectNumberBindMethod(const char* signature)
{
  if (strcmp(signature, "TestObject.id") == 0) return testObjectGetId;
  return NULL;
}

void objectNumberBindClass(const char* className, WrenForeignClassMethods* methods)
{
  if (strcmp(className, "TestObject") == 0)
  {
    methods->allocate = testObjectAllocate;
    methods->finalize = NULL;
  }
}

// The callback function that handles Object Numbers
static void objectNumberCallback(WrenVM* vm, int64_t value)
{
  wrenEnsureSlots(vm, 1);
  // For this test, just return the number as a double
  // A real application could create foreign objects, strings, or any other Wren value
  wrenSetSlotDouble(vm, 0, (double)value);
  // The value is now in slot 0, which the compiler will read
}

int objectNumberRunTests(WrenVM* vm)
{
  // Test that Object Numbers work
  //printf("Object Number basic test passed!\n");
  return 0;
}

WrenVM* objectNumberCreateVM()
{
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.bindForeignMethodFn = APITest_bindForeignMethod;
  config.bindForeignClassFn = APITest_bindForeignClass;
  config.objectNumberFn = objectNumberCallback;

  WrenVM* vm = wrenNewVM(&config);
  return vm;
}

WrenObjectNumberFn objectNumberGetCallback()
{
  return objectNumberCallback;
}
