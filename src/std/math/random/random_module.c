#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include "random_module.h"
#include "random.h"
#include "pigeon.h"
#include "wren_common.h"
#include "wren_vm.h"

// Algorithm types
#define ALG_XOSHIRO256PP    0
#define ALG_XOROSHIRO128PP  1
#define ALG_WYRAND          2
#define ALG_XSHIFT64STAR    3

// PRNG state container
typedef struct {
  int algorithm;
  union {
    xoshiro256pp_state xoshiro256;
    xoroshiro128pp_state xoroshiro128;
    uint64_t wyrand;
    uint64_t xorshift64;
  } state;
} PRNGState;

// Foreign class allocator
void randomStateAllocate(PigeonVM* vm) {
  int algorithm = (int)pigeonGetSlotDouble(vm, 1);
  uint64_t seed = (uint64_t)pigeonGetSlotDouble(vm, 2);
  
  PRNGState* prng = (PRNGState*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(PRNGState));
  prng->algorithm = algorithm;
  
  switch (algorithm) {
    case ALG_XOSHIRO256PP:
      xoshiro256pp_seed(&prng->state.xoshiro256, seed);
      break;
    case ALG_XOROSHIRO128PP:
      xoroshiro128pp_seed(&prng->state.xoroshiro128, seed);
      break;
    case ALG_WYRAND:
      prng->state.wyrand = seed;
      break;
    case ALG_XSHIFT64STAR:
      prng->state.xorshift64 = seed;
      break;
    default:
      xoshiro256pp_seed(&prng->state.xoshiro256, seed);
      prng->algorithm = ALG_XOSHIRO256PP;
  }
}

// Get next uint64
static void randomNextUint64(PigeonVM* vm) {
  PRNGState* prng = (PRNGState*)pigeonGetSlotForeign(vm, 0);
  uint64_t result;
  
  switch (prng->algorithm) {
    case ALG_XOSHIRO256PP:
      result = xoshiro256pp_next(&prng->state.xoshiro256);
      break;
    case ALG_XOROSHIRO128PP:
      result = xoroshiro128pp_next(&prng->state.xoroshiro128);
      break;
    case ALG_WYRAND:
      result = wyrand_next(&prng->state.wyrand);
      break;
    case ALG_XSHIFT64STAR:
      result = xorshift64star_next(&prng->state.xorshift64);
      break;
    default:
      result = xoshiro256pp_next(&prng->state.xoshiro256);
  }
  
  // Return as double (Wren numbers are doubles)
  pigeonSetSlotDouble(vm, 0, (double)result);
}

// Get next double in [0, 1)
static void randomNextDouble(PigeonVM* vm) {
  PRNGState* prng = (PRNGState*)pigeonGetSlotForeign(vm, 0);
  uint64_t raw;
  
  switch (prng->algorithm) {
    case ALG_XOSHIRO256PP:
      raw = xoshiro256pp_next(&prng->state.xoshiro256);
      break;
    case ALG_XOROSHIRO128PP:
      raw = xoroshiro128pp_next(&prng->state.xoroshiro128);
      break;
    case ALG_WYRAND:
      raw = wyrand_next(&prng->state.wyrand);
      break;
    case ALG_XSHIFT64STAR:
      raw = xorshift64star_next(&prng->state.xorshift64);
      break;
    default:
      raw = xoshiro256pp_next(&prng->state.xoshiro256);
  }
  
  pigeonSetSlotDouble(vm, 0, uniform01_double(raw));
}

// Get system time for seeding
static void randomSystemTime(PigeonVM* vm) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t seed = (uint64_t)tv.tv_sec ^ (uint64_t)tv.tv_usec;
  pigeonSetSlotDouble(vm, 0, (double)seed);
}

// No-op init - allocation handles initialization
static void randomInitState(PigeonVM* vm) {
  (void)vm;
}

#include "random.wren.inc"

const char* pigeonRandomModuleSource() {
  return randomModuleSource;
}

PigeonForeignMethodFn pigeonRandomModuleBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                      const char* className,
                                                      bool isStatic,
                                                      const char* signature)
{
  (void)vm;
  
  if (!isStatic) {
    if (strcmp(className, "RandomState") == 0) {
      if (strcmp(signature, "initState_(_,_)") == 0) return randomInitState;
      if (strcmp(signature, "nextUint64_()") == 0) return randomNextUint64;
      if (strcmp(signature, "nextDouble_()") == 0) return randomNextDouble;
    }
    return NULL;
  }
  
  // Static methods
  if (strcmp(className, "Random") == 0) {
    if (strcmp(signature, "systemSeed_()") == 0) return randomSystemTime;
  }
  
  return NULL;
}

PigeonForeignClassMethods pigeonRandomModuleBindForeignClass(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                         const char* className)
{
  (void)vm;
  
  PigeonForeignClassMethods methods = { NULL, NULL };
  
  if (strcmp(className, "RandomState") == 0) {
    methods.allocate = randomStateAllocate;
  }
  
  return methods;
}
