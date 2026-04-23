#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "io.h"
#include "wren.h"
#include "wren_common.h"

// Reader.read_(fd, n)
static void readerRead(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  int n  = (int)wrenGetSlotDouble(vm, 2);
  char* buf = malloc(n + 1);
  ssize_t got = read(fd, buf, n);
  if (got < 0) got = 0;
  buf[got] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
}

// Reader.readAll_(fd)
static void readerReadAll(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  size_t cap = 4096, len = 0;
  char* buf = malloc(cap);
  ssize_t n;
  while ((n = read(fd, buf + len, cap - len)) > 0) {
    len += n;
    if (len == cap) {
      cap *= 2;
      buf = realloc(buf, cap);
    }
  }
  buf[len] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
}

// Reader.readLine_(fd)
static void readerReadLine(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  size_t cap = 256, len = 0;
  char* buf = malloc(cap);
  char c;
  ssize_t n;
  while ((n = read(fd, &c, 1)) == 1) {
    if (len + 1 >= cap) { cap *= 2; buf = realloc(buf, cap); }
    if (c == '\n') break;
    buf[len++] = c;
  }
  buf[len] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
}

// Reader.close_(fd) / Writer.close_(fd)
static void fdClose(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  close(fd);
}

// Writer.write_(fd, s)
static void writerWrite(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  const char* s = wrenGetSlotString(vm, 2);
  size_t len = strlen(s);
  ssize_t written = write(fd, s, len);
  wrenSetSlotDouble(vm, 0, (double)(written < 0 ? 0 : written));
}

// Writer.flush_(fd)
static void writerFlush(WrenVM* vm) {
  // For raw fds, flush is a no-op (no userspace buffer); fsync if needed.
  (void)vm;
}

// Pipe.create_()
static void pipeCreate(WrenVM* vm) {
  int fds[2];
  if (pipe(fds) == -1) {
    wrenSetSlotString(vm, 0, "Failed to create pipe");
    return;
  }
  wrenSetSlotNewList(vm, 0);
  wrenSetSlotDouble(vm, 1, fds[0]);
  wrenInsertInList(vm, 0, -1, 1);
  wrenSetSlotDouble(vm, 1, fds[1]);
  wrenInsertInList(vm, 0, -1, 1);
}

#include "io.wren.inc"

const char* wrenIoSource() {
  return ioModuleSource;
}

WrenForeignMethodFn wrenIoBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature)
{
  if (!isStatic) return NULL;

  if (strcmp(className, "Reader") == 0) {
    if (strcmp(signature, "read_(_,_)") == 0)  return readerRead;
    if (strcmp(signature, "readAll_(_)") == 0) return readerReadAll;
    if (strcmp(signature, "readLine_(_)") == 0) return readerReadLine;
    if (strcmp(signature, "close_(_)") == 0)   return fdClose;
  }

  if (strcmp(className, "Writer") == 0) {
    if (strcmp(signature, "write_(_,_)") == 0) return writerWrite;
    if (strcmp(signature, "flush_(_)") == 0)   return writerFlush;
    if (strcmp(signature, "close_(_)") == 0)   return fdClose;
  }

  if (strcmp(className, "Pipe") == 0) {
    if (strcmp(signature, "create_()") == 0) return pipeCreate;
  }

  return NULL;
}

WrenForeignClassMethods wrenIoBindForeignClass(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* WREN_MAYBE_UNUSED className)
{
  WrenForeignClassMethods methods = { NULL, NULL };
  return methods;
}
