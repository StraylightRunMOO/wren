#ifndef object_number_h
#define object_number_h

#include "pigeon.h"
#include "api_tests.h"

PigeonForeignMethodFn objectNumberBindMethod(const char* signature);
void objectNumberBindClass(const char* className, PigeonForeignClassMethods* methods);
int objectNumberRunTests(PigeonVM* vm);
PigeonVM* objectNumberCreateVM();
PigeonObjectNumberFn objectNumberGetCallback();

#endif
