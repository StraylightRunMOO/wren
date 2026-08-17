#include "pigeon.h"

PigeonForeignMethodFn mapsBindMethod(const char* signature);
void mapBindClass(
    const char* className, PigeonForeignClassMethods* methods);
