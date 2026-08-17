#include <string.h>

#include "user_data.h"

static const char* data = "my user data";
static const char* otherData = "other user data";

void* testReallocateFn(void* ptr, size_t newSize, void* userData) {
  if (strcmp(userData, data) != 0) return NULL;

  if (newSize == 0)
  {
    free(ptr);
    return NULL;
  }

  return realloc(ptr, newSize);
}

static void test(PigeonVM* vm)
{
  PigeonConfiguration configuration;
  pigeonInitConfiguration(&configuration);

  // Should default to NULL.
  if (configuration.userData != NULL)
  {
    pigeonSetSlotBool(vm, 0, false);
    return;
  }

  configuration.reallocateFn = testReallocateFn;
  configuration.userData = (void*)data;

  PigeonVM* otherVM = pigeonNewVM(&configuration);

  // Should be able to get it.
  if (pigeonGetUserData(otherVM) != data)
  {
    pigeonSetSlotBool(vm, 0, false);
    pigeonFreeVM(otherVM);
    return;
  }

  // Should be able to set it.
  pigeonSetUserData(otherVM, (void*)otherData);

  if (pigeonGetUserData(otherVM) != otherData)
  {
    pigeonSetSlotBool(vm, 0, false);
    pigeonFreeVM(otherVM);
    return;
  }

  pigeonSetSlotBool(vm, 0, true);
  pigeonFreeVM(otherVM);
}

PigeonForeignMethodFn userDataBindMethod(const char* signature)
{
  if (strcmp(signature, "static UserData.test") == 0) return test;

  return NULL;
}
