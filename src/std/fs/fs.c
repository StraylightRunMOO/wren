#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "fs.h"
#include "wren.h"
#include "wren_common.h"

// fs.exists_(path)
static void fsExists(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  wrenSetSlotBool(vm, 0, stat(path, &st) == 0);
}

// fs.remove_(path)
static void fsRemove(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  wrenSetSlotBool(vm, 0, remove(path) == 0);
}

// fs.rename_(old, new)
static void fsRename(WrenVM* vm) {
  const char* oldPath = wrenGetSlotString(vm, 1);
  const char* newPath = wrenGetSlotString(vm, 2);
  wrenSetSlotBool(vm, 0, rename(oldPath, newPath) == 0);
}

// fs.size_(path)
static void fsSize(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  wrenSetSlotDouble(vm, 0, stat(path, &st) == 0 ? (double)st.st_size : -1);
}

// File.open_(path, flags)
static void fileOpen(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  int flags = (int)wrenGetSlotDouble(vm, 2);
  // Map Wren flags to open(2) flags
  int oflags = 0;
  if (flags & 2)    oflags |= O_RDWR;
  else if (flags & 1) oflags |= O_WRONLY;
  else              oflags  = O_RDONLY;
  if (flags & 64)   oflags |= O_CREAT;
  if (flags & 512)  oflags |= O_TRUNC;
  if (flags & 1024) oflags |= O_APPEND;
  int fd = open(path, oflags, 0644);
  wrenSetSlotDouble(vm, 0, fd);
}

// File.read_(fd, n) — byte-exact, may contain NULs.
static void fileRead(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  int n  = (int)wrenGetSlotDouble(vm, 2);
  if (n <= 0) {
    wrenSetSlotBytes(vm, 0, "", 0);
    return;
  }
  char* buf = malloc((size_t)n);
  if (buf == NULL) {
    wrenSetSlotString(vm, 0, "Out of memory");
    wrenAbortFiber(vm, 0);
    return;
  }
  ssize_t got = read(fd, buf, (size_t)n);
  if (got < 0) got = 0;
  wrenSetSlotBytes(vm, 0, buf, (size_t)got);
  free(buf);
}

// File.readAll_(fd)
static void fileReadAll(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  size_t cap = 4096, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    wrenSetSlotString(vm, 0, "Out of memory");
    wrenAbortFiber(vm, 0);
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
        wrenSetSlotString(vm, 0, "Out of memory");
        wrenAbortFiber(vm, 0);
        return;
      }
      buf = grown;
    }
  }
  wrenSetSlotBytes(vm, 0, buf, len);
  free(buf);
}

// File.readLine_(fd) — drops the newline; keeps other bytes including NUL.
static void fileReadLine(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  size_t cap = 256, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    wrenSetSlotString(vm, 0, "Out of memory");
    wrenAbortFiber(vm, 0);
    return;
  }
  char c;
  while (read(fd, &c, 1) == 1) {
    if (c == '\n') break;
    if (len + 1 >= cap) {
      cap *= 2;
      char* grown = realloc(buf, cap);
      if (grown == NULL) {
        free(buf);
        wrenSetSlotString(vm, 0, "Out of memory");
        wrenAbortFiber(vm, 0);
        return;
      }
      buf = grown;
    }
    buf[len++] = c;
  }
  wrenSetSlotBytes(vm, 0, buf, len);
  free(buf);
}

// File.write_(fd, s) — writes the string's byte length, not strlen.
static void fileWrite(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  int length = 0;
  const char* s = wrenGetSlotBytes(vm, 2, &length);
  ssize_t written = write(fd, s, (size_t)length);
  wrenSetSlotDouble(vm, 0, (double)(written < 0 ? 0 : written));
}

// File.flush_(fd)
static void fileFlush(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  fsync(fd);
}

// File.seek_(fd, offset, whence)
static void fileSeek(WrenVM* vm) {
  int fd     = (int)wrenGetSlotDouble(vm, 1);
  off_t off  = (off_t)wrenGetSlotDouble(vm, 2);
  int whence = (int)wrenGetSlotDouble(vm, 3);
  wrenSetSlotDouble(vm, 0, (double)lseek(fd, off, whence));
}

// File.tell_(fd)
static void fileTell(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  wrenSetSlotDouble(vm, 0, (double)lseek(fd, 0, SEEK_CUR));
}

// File.close_(fd)
static void fileClose(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  close(fd);
}

#include "fs.wren.inc"

const char* wrenFsSource() {
  return fsModuleSource;
}

WrenForeignMethodFn wrenFsBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature)
{
  if (!isStatic) return NULL;

  if (strcmp(className, "fs") == 0) {
    if (strcmp(signature, "exists_(_)") == 0)      return fsExists;
    if (strcmp(signature, "remove_(_)") == 0)      return fsRemove;
    if (strcmp(signature, "rename_(_,_)") == 0)    return fsRename;
    if (strcmp(signature, "size_(_)") == 0)        return fsSize;
  }

  if (strcmp(className, "File") == 0) {
    if (strcmp(signature, "exists_(_)") == 0)   return fsExists;
    if (strcmp(signature, "remove_(_)") == 0)   return fsRemove;
    if (strcmp(signature, "rename_(_,_)") == 0) return fsRename;
    if (strcmp(signature, "size_(_)") == 0)     return fsSize;
    if (strcmp(signature, "open_(_,_)") == 0)   return fileOpen;
    if (strcmp(signature, "read_(_,_)") == 0)   return fileRead;
    if (strcmp(signature, "readAll_(_)") == 0)  return fileReadAll;
    if (strcmp(signature, "readLine_(_)") == 0) return fileReadLine;
    if (strcmp(signature, "write_(_,_)") == 0)  return fileWrite;
    if (strcmp(signature, "flush_(_)") == 0)    return fileFlush;
    if (strcmp(signature, "seek_(_,_,_)") == 0) return fileSeek;
    if (strcmp(signature, "tell_(_)") == 0)     return fileTell;
    if (strcmp(signature, "close_(_)") == 0)    return fileClose;
  }

  return NULL;
}

WrenForeignClassMethods wrenFsBindForeignClass(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* WREN_MAYBE_UNUSED className)
{
  WrenForeignClassMethods methods = { NULL, NULL };
  return methods;
}
