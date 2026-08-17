#include "pigeon.h"

PigeonForeignMethodFn foreignClassBindMethod(const char* signature);
void foreignClassBindClass(
    const char* className, PigeonForeignClassMethods* methods);
