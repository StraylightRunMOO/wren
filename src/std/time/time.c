#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "time.h"
#include "pigeon.h"
#include "wren_common.h"
#include "wren_vm.h"

// Get current time as Unix timestamp
static void timeNow(PigeonVM* vm) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  pigeonSetSlotDouble(vm, 0, (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0);
}

// Get current time in milliseconds
static void timeNowMillis(PigeonVM* vm) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  pigeonSetSlotDouble(vm, 0, (double)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

// Get current time in nanoseconds
static void timeNowNanos(PigeonVM* vm) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  pigeonSetSlotDouble(vm, 0, (double)(ts.tv_sec * 1000000000LL + ts.tv_nsec));
}

// Sleep for specified seconds
static void timeSleep(PigeonVM* vm) {
  double seconds = pigeonGetSlotDouble(vm, 1);
  usleep((useconds_t)(seconds * 1000000));
}

// Parse time from string
static void timeParse(PigeonVM* vm) {
  const char* layout = pigeonGetSlotString(vm, 1);
  const char* value = pigeonGetSlotString(vm, 2);
  
  // Simplified parsing - support common layouts
  struct tm tm = {0};
  
  if (strcmp(layout, "2006-01-02 15:04:05") == 0) {
    sscanf(value, "%d-%d-%d %d:%d:%d", 
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
  } else if (strcmp(layout, "2006-01-02") == 0) {
    sscanf(value, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
  } else if (strcmp(layout, "15:04:05") == 0) {
    sscanf(value, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
  } else {
    // Try RFC3339
    strptime(value, "%Y-%m-%dT%H:%M:%S", &tm);
  }
  
  time_t t = mktime(&tm);
  pigeonSetSlotDouble(vm, 0, (double)t);
}

// Create time from components
static void timeFromComponents(PigeonVM* vm) {
  int year = (int)pigeonGetSlotDouble(vm, 1);
  int month = (int)pigeonGetSlotDouble(vm, 2);
  int day = (int)pigeonGetSlotDouble(vm, 3);
  int hour = (int)pigeonGetSlotDouble(vm, 4);
  int min = (int)pigeonGetSlotDouble(vm, 5);
  int sec = (int)pigeonGetSlotDouble(vm, 6);
  
  struct tm tm = {0};
  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = min;
  tm.tm_sec = sec;
  
  time_t t = mktime(&tm);
  pigeonSetSlotDouble(vm, 0, (double)t);
}

// Get time component (0=year, 1=month, ..., 7=yearday)
static void timeGetComponent(PigeonVM* vm) {
  double sec = pigeonGetSlotDouble(vm, 1);
  int idx = (int)pigeonGetSlotDouble(vm, 2);
  
  time_t t = (time_t)sec;
  struct tm* tm = localtime(&t);
  
  double result;
  switch (idx) {
    case 0: result = tm->tm_year + 1900; break;
    case 1: result = tm->tm_mon + 1; break;
    case 2: result = tm->tm_mday; break;
    case 3: result = tm->tm_hour; break;
    case 4: result = tm->tm_min; break;
    case 5: result = tm->tm_sec; break;
    case 6: result = tm->tm_wday; break;
    case 7: result = tm->tm_yday; break;
    default: result = 0;
  }
  
  pigeonSetSlotDouble(vm, 0, result);
}

// Format time
static void timeFormat(PigeonVM* vm) {
  double sec = pigeonGetSlotDouble(vm, 1);
  const char* layout = pigeonGetSlotString(vm, 2);
  
  time_t t = (time_t)sec;
  struct tm* tm = localtime(&t);
  
  char buf[256];
  
  // Map Go-style layouts to strftime formats
  if (strcmp(layout, "2006-01-02 15:04:05") == 0) {
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
  } else if (strcmp(layout, "2006-01-02") == 0) {
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
  } else if (strcmp(layout, "15:04:05") == 0) {
    strftime(buf, sizeof(buf), "%H:%M:%S", tm);
  } else if (strcmp(layout, "2006-01-02T15:04:05") == 0) {
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm);
  } else if (strcmp(layout, "Mon, 02 Jan 2006 15:04:05 MST") == 0) {
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", tm);
  } else {
    // Default
    strftime(buf, sizeof(buf), "%c", tm);
  }
  
  pigeonSetSlotString(vm, 0, buf);
}

#include "time.wren.inc"

const char* pigeonTimeSource() {
  return timeModuleSource;
}

PigeonForeignMethodFn pigeonTimeBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature)
{
  (void)vm;
  
  if (strcmp(className, "Time") != 0) return NULL;
  
  if (isStatic) {
    if (strcmp(signature, "now()") == 0) return timeNow;
    if (strcmp(signature, "nowMillis()") == 0) return timeNowMillis;
    if (strcmp(signature, "nowNanos()") == 0) return timeNowNanos;
    if (strcmp(signature, "sleep(_)") == 0) return timeSleep;
    if (strcmp(signature, "parse_(_,_)") == 0) return timeParse;
    if (strcmp(signature, "fromComponents_(_,_,_,_,_,_)") == 0) return timeFromComponents;
    if (strcmp(signature, "getComponent_(_,_)") == 0) return timeGetComponent;
    if (strcmp(signature, "format_(_,_)") == 0) return timeFormat;
  }
  
  return NULL;
}
