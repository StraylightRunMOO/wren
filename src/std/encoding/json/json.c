#define _GNU_SOURCE
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "pigeon.h"
#include "wren_common.h"

#define JSON_MAX_SLOTS 64

typedef struct {
  PigeonVM* vm;
  const char* src;
  int len;
  int pos;
  const char* err;
} JsonParser;

static bool jsonIsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static void jsonSkip(JsonParser* p) {
  while (p->pos < p->len && jsonIsSpace(p->src[p->pos])) p->pos++;
}

static bool jsonTake(JsonParser* p, char c) {
  jsonSkip(p);
  if (p->pos < p->len && p->src[p->pos] == c) {
    p->pos++;
    return true;
  }
  return false;
}

static bool jsonFail(JsonParser* p, const char* msg) {
  p->err = msg;
  return false;
}

static int hexVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

static int utf8Append(char* out, int cp) {
  if (cp <= 0x7F) {
    out[0] = (char)cp;
    return 1;
  }
  if (cp <= 0x7FF) {
    out[0] = (char)(0xC0 | (cp >> 6));
    out[1] = (char)(0x80 | (cp & 0x3F));
    return 2;
  }
  if (cp <= 0xFFFF) {
    out[0] = (char)(0xE0 | (cp >> 12));
    out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[2] = (char)(0x80 | (cp & 0x3F));
    return 3;
  }
  out[0] = (char)(0xF0 | (cp >> 18));
  out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
  out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
  out[3] = (char)(0x80 | (cp & 0x3F));
  return 4;
}

static bool parseValue(JsonParser* p, int outSlot);

static bool parseString(JsonParser* p, int outSlot) {
  jsonSkip(p);
  if (!jsonTake(p, '"')) return jsonFail(p, "Expected string");

  size_t cap = 32, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) return jsonFail(p, "Out of memory");

  while (p->pos < p->len) {
    unsigned char c = (unsigned char)p->src[p->pos++];
    if (c == '"') {
      pigeonSetSlotBytes(p->vm, outSlot, buf, len);
      free(buf);
      return true;
    }
    if (c < 0x20) {
      free(buf);
      return jsonFail(p, "Unescaped control character in string");
    }

    char tmp[4];
    const char* add = tmp;
    int addLen = 1;
    if (c == '\\') {
      if (p->pos >= p->len) {
        free(buf);
        return jsonFail(p, "Unterminated string escape");
      }
      char e = p->src[p->pos++];
      switch (e) {
        case '"': case '\\': case '/': tmp[0] = e; break;
        case 'b': tmp[0] = '\b'; break;
        case 'f': tmp[0] = '\f'; break;
        case 'n': tmp[0] = '\n'; break;
        case 'r': tmp[0] = '\r'; break;
        case 't': tmp[0] = '\t'; break;
        case 'u': {
          if (p->pos + 4 > p->len) {
            free(buf);
            return jsonFail(p, "Truncated \\u escape");
          }
          int cp = 0;
          for (int i = 0; i < 4; i++) {
            int h = hexVal(p->src[p->pos++]);
            if (h < 0) {
              free(buf);
              return jsonFail(p, "Invalid \\u escape");
            }
            cp = (cp << 4) | h;
          }
          addLen = utf8Append(tmp, cp);
          break;
        }
        default:
          free(buf);
          return jsonFail(p, "Invalid string escape");
      }
    } else {
      tmp[0] = (char)c;
    }

    if (len + (size_t)addLen > cap) {
      cap = (len + (size_t)addLen) * 2;
      char* grown = realloc(buf, cap);
      if (grown == NULL) {
        free(buf);
        return jsonFail(p, "Out of memory");
      }
      buf = grown;
    }
    memcpy(buf + len, add, (size_t)addLen);
    len += (size_t)addLen;
  }

  free(buf);
  return jsonFail(p, "Unterminated string");
}

static bool jsonIsDigit(char c) {
  return c >= '0' && c <= '9';
}

static bool parseNumber(JsonParser* p, int outSlot) {
  jsonSkip(p);
  int start = p->pos;
  if (p->pos < p->len && p->src[p->pos] == '-') p->pos++;
  if (p->pos >= p->len || !jsonIsDigit(p->src[p->pos])) {
    return jsonFail(p, "Invalid number");
  }
  if (p->src[p->pos] == '0') {
    p->pos++;
  } else {
    while (p->pos < p->len && jsonIsDigit(p->src[p->pos])) p->pos++;
  }
  if (p->pos < p->len && p->src[p->pos] == '.') {
    p->pos++;
    if (p->pos >= p->len || !jsonIsDigit(p->src[p->pos])) {
      return jsonFail(p, "Invalid number");
    }
    while (p->pos < p->len && jsonIsDigit(p->src[p->pos])) p->pos++;
  }
  if (p->pos < p->len && (p->src[p->pos] == 'e' || p->src[p->pos] == 'E')) {
    p->pos++;
    if (p->pos < p->len && (p->src[p->pos] == '+' || p->src[p->pos] == '-')) {
      p->pos++;
    }
    if (p->pos >= p->len || !jsonIsDigit(p->src[p->pos])) {
      return jsonFail(p, "Invalid number");
    }
    while (p->pos < p->len && jsonIsDigit(p->src[p->pos])) p->pos++;
  }

  char* tmp = malloc((size_t)(p->pos - start) + 1);
  if (tmp == NULL) return jsonFail(p, "Out of memory");
  memcpy(tmp, p->src + start, (size_t)(p->pos - start));
  tmp[p->pos - start] = '\0';
  char* end = NULL;
  double n = strtod(tmp, &end);
  bool ok = end != NULL && end != tmp && *end == '\0';
  free(tmp);
  if (!ok) return jsonFail(p, "Invalid number");
  pigeonSetSlotDouble(p->vm, outSlot, n);
  return true;
}

static bool parseLiteral(JsonParser* p, const char* lit, int outSlot) {
  jsonSkip(p);
  int n = (int)strlen(lit);
  if (p->pos + n > p->len || memcmp(p->src + p->pos, lit, (size_t)n) != 0) {
    return jsonFail(p, "Expected literal");
  }
  p->pos += n;
  if (lit[0] == 'n') {
    pigeonSetSlotNull(p->vm, outSlot);
  } else {
    pigeonSetSlotBool(p->vm, outSlot, lit[0] == 't');
  }
  return true;
}

static bool parseArray(JsonParser* p, int outSlot) {
  if (!jsonTake(p, '[')) return jsonFail(p, "Expected '['");
  if (outSlot + 2 >= JSON_MAX_SLOTS) return jsonFail(p, "JSON nesting too deep");
  pigeonSetSlotNewList(p->vm, outSlot);
  if (jsonTake(p, ']')) return true;

  int elem = outSlot + 1;
  while (true) {
    if (!parseValue(p, elem)) return false;
    pigeonInsertInList(p->vm, outSlot, -1, elem);
    if (jsonTake(p, ']')) return true;
    if (!jsonTake(p, ',')) return jsonFail(p, "Expected ',' or ']' in array");
  }
}

static bool parseObject(JsonParser* p, int outSlot) {
  if (!jsonTake(p, '{')) return jsonFail(p, "Expected '{'");
  if (outSlot + 3 >= JSON_MAX_SLOTS) return jsonFail(p, "JSON nesting too deep");
  pigeonSetSlotNewMap(p->vm, outSlot);
  if (jsonTake(p, '}')) return true;

  int keySlot = outSlot + 1;
  int valSlot = outSlot + 2;
  while (true) {
    if (!parseString(p, keySlot)) return false;
    if (!jsonTake(p, ':')) return jsonFail(p, "Expected ':' after object key");
    if (!parseValue(p, valSlot)) return false;
    pigeonSetMapValue(p->vm, outSlot, keySlot, valSlot);
    if (jsonTake(p, '}')) return true;
    if (!jsonTake(p, ',')) return jsonFail(p, "Expected ',' or '}' in object");
  }
}

static bool parseValue(JsonParser* p, int outSlot) {
  jsonSkip(p);
  if (p->pos >= p->len) return jsonFail(p, "Unexpected end of JSON");
  char c = p->src[p->pos];
  if (c == '"') return parseString(p, outSlot);
  if (c == '{') return parseObject(p, outSlot);
  if (c == '[') return parseArray(p, outSlot);
  if (c == 't') return parseLiteral(p, "true", outSlot);
  if (c == 'f') return parseLiteral(p, "false", outSlot);
  if (c == 'n') return parseLiteral(p, "null", outSlot);
  if (c == '-' || jsonIsDigit(c)) return parseNumber(p, outSlot);
  return jsonFail(p, "Unexpected character");
}

static void jsonParse(PigeonVM* vm) {
  if (pigeonGetSlotType(vm, 1) != PIGEON_TYPE_STRING) {
    pigeonSetSlotString(vm, 0, "JSON text must be a string");
    pigeonAbortFiber(vm, 0);
    return;
  }

  int length = 0;
  const char* text = pigeonGetSlotBytes(vm, 1, &length);
  char* copy = malloc((size_t)length + 1);
  if (copy == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
    return;
  }
  if (length > 0) memcpy(copy, text, (size_t)length);
  copy[length] = '\0';

  JsonParser p;
  p.vm = vm;
  p.src = copy;
  p.len = length;
  p.pos = 0;
  p.err = NULL;

  pigeonEnsureSlots(vm, JSON_MAX_SLOTS);
  if (!parseValue(&p, 0)) {
    pigeonSetSlotString(vm, 0, p.err ? p.err : "Invalid JSON");
    pigeonAbortFiber(vm, 0);
    free(copy);
    return;
  }
  jsonSkip(&p);
  if (p.pos < p.len) {
    pigeonSetSlotString(vm, 0, "Trailing data after JSON value");
    pigeonAbortFiber(vm, 0);
    free(copy);
    return;
  }
  free(copy);
}

static void jsonEncodeNum(PigeonVM* vm) {
  double n = pigeonGetSlotDouble(vm, 1);
  if (!isfinite(n)) {
    pigeonSetSlotString(vm, 0, "Cannot encode NaN or Infinity as JSON");
    pigeonAbortFiber(vm, 0);
    return;
  }
  char buf[64];
  int written = snprintf(buf, sizeof(buf), "%.17g", n);
  if (written < 0 || written >= (int)sizeof(buf)) {
    pigeonSetSlotString(vm, 0, "Number format failed");
    pigeonAbortFiber(vm, 0);
    return;
  }
  pigeonSetSlotBytes(vm, 0, buf, (size_t)written);
}

static void jsonEncodeString(PigeonVM* vm) {
  int length = 0;
  const char* s = pigeonGetSlotBytes(vm, 1, &length);
  size_t cap = (size_t)length * 6 + 2;
  char* buf = malloc(cap);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
    return;
  }

  size_t n = 0;
  buf[n++] = '"';
  for (int i = 0; i < length; i++) {
    unsigned char c = (unsigned char)s[i];
    const char* esc = NULL;
    char uesc[8];
    int elen = 0;
    switch (c) {
      case '"':  esc = "\\\""; elen = 2; break;
      case '\\': esc = "\\\\"; elen = 2; break;
      case '\b': esc = "\\b";  elen = 2; break;
      case '\f': esc = "\\f";  elen = 2; break;
      case '\n': esc = "\\n";  elen = 2; break;
      case '\r': esc = "\\r";  elen = 2; break;
      case '\t': esc = "\\t";  elen = 2; break;
      default:
        if (c < 0x20) {
          snprintf(uesc, sizeof(uesc), "\\u%04x", c);
          esc = uesc;
          elen = 6;
        } else {
          buf[n++] = (char)c;
          continue;
        }
    }
    memcpy(buf + n, esc, (size_t)elen);
    n += (size_t)elen;
  }
  buf[n++] = '"';
  pigeonSetSlotBytes(vm, 0, buf, n);
  free(buf);
}

#include "json.wren.inc"

const char* pigeonJsonSource() {
  return jsonModuleSource;
}

PigeonForeignMethodFn pigeonJsonBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature)
{
  if (!isStatic || strcmp(className, "Json") != 0) return NULL;
  if (strcmp(signature, "parse_(_)") == 0) return jsonParse;
  if (strcmp(signature, "encodeNum_(_)") == 0) return jsonEncodeNum;
  if (strcmp(signature, "encodeString_(_)") == 0) return jsonEncodeString;
  return NULL;
}

PigeonForeignClassMethods pigeonJsonBindForeignClass(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                                 const char* PIGEON_MAYBE_UNUSED className)
{
  PigeonForeignClassMethods methods = { NULL, NULL };
  return methods;
}
