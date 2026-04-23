#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "math.h"
#include "wren.h"
#include "wren_common.h"

// Gamma function
static void mathGamma(WrenVM* vm) {
  double x = wrenGetSlotDouble(vm, 1);
  #if defined(__GLIBC__) || defined(__APPLE__) || defined(__FreeBSD__)
    wrenSetSlotDouble(vm, 0, tgamma(x));
  #else
    // Fallback to lgamma
    wrenSetSlotDouble(vm, 0, exp(lgamma(x)));
  #endif
}

// Log gamma function
static void mathLgamma(WrenVM* vm) {
  double x = wrenGetSlotDouble(vm, 1);
  wrenSetSlotDouble(vm, 0, lgamma(x));
}

// Error function
static void mathErf(WrenVM* vm) {
  double x = wrenGetSlotDouble(vm, 1);
  #if defined(__GLIBC__) || defined(__APPLE__) || defined(__FreeBSD__)
    wrenSetSlotDouble(vm, 0, erf(x));
  #else
    // Abramowitz and Stegun approximation
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;
    
    int sign = (x < 0) ? -1 : 1;
    x = fabs(x);
    
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
    
    wrenSetSlotDouble(vm, 0, sign * y);
  #endif
}

// System time for seeding
static void mathSystemTime(WrenVM* vm) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t seed = (uint64_t)tv.tv_sec ^ (uint64_t)tv.tv_usec;
  wrenSetSlotDouble(vm, 0, (double)seed);
}

#include "math.wren.inc"

const char* wrenMathModuleSource() {
  return mathModuleSource;
}

WrenForeignMethodFn wrenMathModuleBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                                    const char* className,
                                                    bool isStatic,
                                                    const char* signature)
{
  (void)vm;
  
  if (!isStatic) return NULL;
  
  if (strcmp(className, "math") == 0) {
    if (strcmp(signature, "foreignGamma_(_)") == 0) return mathGamma;
    if (strcmp(signature, "foreignLgamma_(_)") == 0) return mathLgamma;
    if (strcmp(signature, "foreignErf_(_)") == 0) return mathErf;
    if (strcmp(signature, "systemTime_()") == 0) return mathSystemTime;
  }
  
  if (strcmp(className, "Rand") == 0) {
    if (strcmp(signature, "systemTime_()") == 0) return mathSystemTime;
  }
  
  return NULL;
}
