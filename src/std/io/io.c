#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "io.h"
#include "pigeon.h"
#include "wren_common.h"

// Reader.read_(fd, n) — byte-exact, may contain NULs.
static void readerRead(PigeonVM* vm) {
  int fd = (int)pigeonGetSlotDouble(vm, 1);
  int n  = (int)pigeonGetSlotDouble(vm, 2);
  if (n <= 0) {
    pigeonSetSlotBytes(vm, 0, "", 0);
    return;
  }
  char* buf = malloc((size_t)n);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
    return;
  }
  ssize_t got = read(fd, buf, (size_t)n);
  if (got < 0) got = 0;
  pigeonSetSlotBytes(vm, 0, buf, (size_t)got);
  free(buf);
}

// Reader.readAll_(fd)
static void readerReadAll(PigeonVM* vm) {
  int fd = (int)pigeonGetSlotDouble(vm, 1);
  size_t cap = 4096, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
    return;
  }
  ssize_t n;
  while ((n = read(fd, buf + len, cap - len)) > 0) {
    len += (size_t)n;
    if (len == cap) {
      cap *= 2;
      char* grown = realloc(buf, cap);
      if (grown == NULL) {
        free(buf);
        pigeonSetSlotString(vm, 0, "Out of memory");
        pigeonAbortFiber(vm, 0);
        return;
      }
      buf = grown;
    }
  }
  pigeonSetSlotBytes(vm, 0, buf, len);
  free(buf);
}

// Reader.readLine_(fd)
static void readerReadLine(PigeonVM* vm) {
  int fd = (int)pigeonGetSlotDouble(vm, 1);
  size_t cap = 256, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
    return;
  }
  char c;
  ssize_t n;
  while ((n = read(fd, &c, 1)) == 1) {
    if (c == '\n') break;
    if (len + 1 >= cap) {
      cap *= 2;
      char* grown = realloc(buf, cap);
      if (grown == NULL) {
        free(buf);
        pigeonSetSlotString(vm, 0, "Out of memory");
        pigeonAbortFiber(vm, 0);
        return;
      }
      buf = grown;
    }
    buf[len++] = c;
  }
  pigeonSetSlotBytes(vm, 0, buf, len);
  free(buf);
}

// Reader.close_(fd) / Writer.close_(fd)
static void fdClose(PigeonVM* vm) {
  int fd = (int)pigeonGetSlotDouble(vm, 1);
  close(fd);
}

// Writer.write_(fd, s) — writes the string's byte length, not strlen.
static void writerWrite(PigeonVM* vm) {
  int fd = (int)pigeonGetSlotDouble(vm, 1);
  int length = 0;
  const char* s = pigeonGetSlotBytes(vm, 2, &length);
  ssize_t written = write(fd, s, (size_t)length);
  pigeonSetSlotDouble(vm, 0, (double)(written < 0 ? 0 : written));
}

// Writer.flush_(fd)
static void writerFlush(PigeonVM* vm) {
  // For raw fds, flush is a no-op (no userspace buffer); fsync if needed.
  (void)vm;
}

// Pipe.create_()
static void pipeCreate(PigeonVM* vm) {
  int fds[2];
  if (pipe(fds) == -1) {
    pigeonSetSlotString(vm, 0, "Failed to create pipe");
    return;
  }
  pigeonSetSlotNewList(vm, 0);
  pigeonSetSlotDouble(vm, 1, fds[0]);
  pigeonInsertInList(vm, 0, -1, 1);
  pigeonSetSlotDouble(vm, 1, fds[1]);
  pigeonInsertInList(vm, 0, -1, 1);
}

#include "io.wren.inc"

const char* pigeonIoSource() {
  return ioModuleSource;
}

PigeonForeignMethodFn pigeonIoBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
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

PigeonForeignClassMethods pigeonIoBindForeignClass(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                               const char* PIGEON_MAYBE_UNUSED className)
{
  PigeonForeignClassMethods methods = { NULL, NULL };
  return methods;
}
