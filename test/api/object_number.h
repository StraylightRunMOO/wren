#ifndef object_number_h
#define object_number_h

#include "wren.h"
#include "api_tests.h"

WrenForeignMethodFn objectNumberBindMethod(const char* signature);
void objectNumberBindClass(const char* className, WrenForeignClassMethods* methods);
int objectNumberRunTests(WrenVM* vm);
WrenVM* objectNumberCreateVM();
WrenObjectNumberFn objectNumberGetCallback();

#endif
