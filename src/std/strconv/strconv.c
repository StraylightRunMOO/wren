#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "strconv.h"
#include "pigeon.h"
#include "wren_common.h"
#include "wren_vm.h"

// Parse integer with base
static void strconvParseInt(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  int base = (int)pigeonGetSlotDouble(vm, 2);
  
  char* end;
  errno = 0;
  long val = strtol(s, &end, base);
  
  if (errno != 0 || *end != '\0') {
    vm->fiber->error = pigeonNewString(vm, "Invalid integer");
    return;
  }
  
  pigeonSetSlotDouble(vm, 0, (double)val);
}

// Parse float
static void strconvParseFloat(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  
  char* end;
  errno = 0;
  double val = strtod(s, &end);
  
  if (errno != 0 || *end != '\0') {
    vm->fiber->error = pigeonNewString(vm, "Invalid float");
    return;
  }
  
  pigeonSetSlotDouble(vm, 0, val);
}

// Format integer
static void strconvFormatInt(PigeonVM* vm) {
  double n = pigeonGetSlotDouble(vm, 1);
  int base = (int)pigeonGetSlotDouble(vm, 2);
  
  char buf[64];
  const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
  
  long val = (long)n;
  int negative = val < 0;
  unsigned long uval = negative ? -val : val;
  
  int i = 0;
  do {
    buf[i++] = digits[uval % base];
    uval /= base;
  } while (uval > 0);
  
  if (negative) buf[i++] = '-';
  buf[i] = '\0';
  
  // Reverse
  for (int j = 0; j < i / 2; j++) {
    char tmp = buf[j];
    buf[j] = buf[i - 1 - j];
    buf[i - 1 - j] = tmp;
  }
  
  pigeonSetSlotString(vm, 0, buf);
}

// Format float
static void strconvFormatFloat(PigeonVM* vm) {
  double n = pigeonGetSlotDouble(vm, 1);
  int prec = (int)pigeonGetSlotDouble(vm, 2);
  int fmt = (int)pigeonGetSlotDouble(vm, 3);
  
  char buf[256];
  const char* format;
  
  if (prec < 0) {
    format = "%g";
    snprintf(buf, sizeof(buf), format, n);
  } else {
    format = "%.*f";
    snprintf(buf, sizeof(buf), format, prec, n);
  }
  
  pigeonSetSlotString(vm, 0, buf);
}

// Quote string
static void strconvQuote(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  size_t len = strlen(s);
  
  // Calculate needed size
  size_t needed = 2; // quotes
  for (size_t i = 0; i < len; i++) {
    switch (s[i]) {
      case '"': case '\\': case '\n': case '\r': case '\t':
        needed += 2;
        break;
      default:
        needed++;
    }
  }
  
  char* buf = (char*)malloc(needed + 1);
  if (!buf) {
    pigeonSetSlotNull(vm, 0);
    return;
  }
  
  size_t j = 0;
  buf[j++] = '"';
  for (size_t i = 0; i < len; i++) {
    switch (s[i]) {
      case '"': buf[j++] = '\\'; buf[j++] = '"'; break;
      case '\\': buf[j++] = '\\'; buf[j++] = '\\'; break;
      case '\n': buf[j++] = '\\'; buf[j++] = 'n'; break;
      case '\r': buf[j++] = '\\'; buf[j++] = 'r'; break;
      case '\t': buf[j++] = '\\'; buf[j++] = 't'; break;
      default: buf[j++] = s[i];
    }
  }
  buf[j++] = '"';
  buf[j] = '\0';
  
  pigeonSetSlotString(vm, 0, buf);
  free(buf);
}

// Unquote string
static void strconvUnquote(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  size_t len = strlen(s);
  
  if (len < 2 || s[0] != '"' || s[len - 1] != '"') {
    vm->fiber->error = pigeonNewString(vm, "Invalid quoted string");
    return;
  }
  
  char* buf = (char*)malloc(len);
  if (!buf) {
    pigeonSetSlotNull(vm, 0);
    return;
  }
  
  size_t j = 0;
  for (size_t i = 1; i < len - 1; i++) {
    if (s[i] == '\\' && i < len - 2) {
      switch (s[i + 1]) {
        case '"': buf[j++] = '"'; i++; break;
        case '\\': buf[j++] = '\\'; i++; break;
        case 'n': buf[j++] = '\n'; i++; break;
        case 'r': buf[j++] = '\r'; i++; break;
        case 't': buf[j++] = '\t'; i++; break;
        default: buf[j++] = s[i];
      }
    } else {
      buf[j++] = s[i];
    }
  }
  buf[j] = '\0';
  
  pigeonSetSlotString(vm, 0, buf);
  free(buf);
}

// Check if valid int
static void strconvIsInt(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  char* end;
  errno = 0;
  strtol(s, &end, 10);
  pigeonSetSlotBool(vm, 0, errno == 0 && *end == '\0');
}

// Check if valid float
static void strconvIsFloat(PigeonVM* vm) {
  const char* s = pigeonGetSlotString(vm, 1);
  char* end;
  errno = 0;
  strtod(s, &end);
  pigeonSetSlotBool(vm, 0, errno == 0 && *end == '\0');
}

#include "strconv.wren.inc"

const char* pigeonStrconvSource() {
  return strconvModuleSource;
}

PigeonForeignMethodFn pigeonStrconvBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                 const char* className,
                                                 bool isStatic,
                                                 const char* signature)
{
  (void)vm;
  
  if (strcmp(className, "strconv") != 0 || !isStatic) return NULL;
  
  if (strcmp(signature, "parseInt_(_,_)") == 0) return strconvParseInt;
  if (strcmp(signature, "parseFloat_(_)") == 0) return strconvParseFloat;
  if (strcmp(signature, "formatInt_(_,_)") == 0) return strconvFormatInt;
  if (strcmp(signature, "formatFloat_(_,_,_)") == 0) return strconvFormatFloat;
  if (strcmp(signature, "quote_(_)") == 0) return strconvQuote;
  if (strcmp(signature, "unquote_(_)") == 0) return strconvUnquote;
  if (strcmp(signature, "isInt_(_)") == 0) return strconvIsInt;
  if (strcmp(signature, "isFloat_(_)") == 0) return strconvIsFloat;
  
  return NULL;
}
