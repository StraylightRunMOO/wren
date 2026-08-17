#pragma once
#ifndef PIGEON_API_TESTS_H
#define PIGEON_API_TESTS_H

#include <stdio.h>
#include <string.h>

#include "pigeon.h"

#include "benchmark.h"
#include "call.h"
#include "call_calls_foreign.h"
#include "call_wren_call_root.h"
#include "error.h"
#include "get_variable.h"
#include "foreign_class.h"
#include "handle.h"
#include "object_number.h"
#include "lists.h"
#include "maps.h"
#include "new_vm.h"
#include "reset_stack_after_call_abort.h"
#include "reset_stack_after_foreign_construct.h"
#include "resolution.h"
#include "slots.h"
#include "user_data.h"

int APITest_Run(PigeonVM* vm, const char* inTestName);

PigeonForeignMethodFn APITest_bindForeignMethod(
    PigeonVM* vm, const char* module, const char* className,
    bool isStatic, const char* signature);

PigeonForeignClassMethods APITest_bindForeignClass(
    PigeonVM* vm, const char* module, const char* className);


#endif //PIGEON_API_TESTS_H