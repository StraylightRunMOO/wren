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

// Foreign data for File instances.
typedef struct {
  int fd;
} FileData;

// ---- fs static methods (filesystem operations, no File instance) ----------

static void fsExists(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  wrenSetSlotBool(vm, 0, stat(path, &st) == 0);
}

static void fsRemove(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  wrenSetSlotBool(vm, 0, remove(path) == 0);
}

static void fsRename(WrenVM* vm) {
  const char* oldPath = wrenGetSlotString(vm, 1);
  const char* newPath = wrenGetSlotString(vm, 2);
  wrenSetSlotBool(vm, 0, rename(oldPath, newPath) == 0);
}

static void fsSize(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  wrenSetSlotDouble(vm, 0, stat(path, &st) == 0 ? (double)st.st_size : -1);
}

// ---- File foreign class ---------------------------------------------------

static void fileAllocate(WrenVM* vm) {
  FileData* data = (FileData*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(FileData));
  data->fd = -1;
}

static void fileFinalize(void* data) {
  FileData* file = (FileData*)data;
  if (file->fd >= 0) {
    close(file->fd);
    file->fd = -1;
  }
}

// ---- File O_* constants ---------------------------------------------------

static void fileORdonly(WrenVM* vm) { wrenSetSlotDouble(vm, 0, O_RDONLY); }
static void fileOWronly(WrenVM* vm) { wrenSetSlotDouble(vm, 0, O_WRONLY); }
static void fileORdwr(WrenVM* vm)   { wrenSetSlotDouble(vm, 0, O_RDWR); }
static void fileOCreate(WrenVM* vm) { wrenSetSlotDouble(vm, 0, O_CREAT); }
static void fileOTrunc(WrenVM* vm)  { wrenSetSlotDouble(vm, 0, O_TRUNC); }
static void fileOAppend(WrenVM* vm) { wrenSetSlotDouble(vm, 0, O_APPEND); }

// ---- File mode parsing (C side, used by constructor) ----------------------

static int parseMode(const char* mode) {
  if (strcmp(mode, "r") == 0)  return O_RDONLY;
  if (strcmp(mode, "w") == 0)  return O_WRONLY | O_CREAT | O_TRUNC;
  if (strcmp(mode, "a") == 0)  return O_WRONLY | O_CREAT | O_APPEND;
  if (strcmp(mode, "r+") == 0) return O_RDWR;
  if (strcmp(mode, "w+") == 0) return O_RDWR | O_CREAT | O_TRUNC;
  if (strcmp(mode, "a+") == 0) return O_RDWR | O_CREAT | O_APPEND;
  return -1;
}

// File.new(path, mode) — foreign constructor body
static void fileInit(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  const char* path = wrenGetSlotString(vm, 1);
  const char* mode = wrenGetSlotString(vm, 2);

  int flags = parseMode(mode);
  if (flags == -1) {
    wrenSetSlotString(vm, 0, "Invalid file mode");
    wrenAbortFiber(vm, 0);
    return;
  }

  data->fd = open(path, flags, 0644);
  if (data->fd < 0) {
    wrenSetSlotString(vm, 0, "Failed to open file");
    wrenAbortFiber(vm, 0);
  }
}

// ---- File instance methods ------------------------------------------------

static void fileFd(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, data->fd);
}

static void fileRead(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  int n = (int)wrenGetSlotDouble(vm, 1);
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
  ssize_t got = read(data->fd, buf, (size_t)n);
  if (got < 0) got = 0;
  wrenSetSlotBytes(vm, 0, buf, (size_t)got);
  free(buf);
}

static void fileReadAll(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  size_t cap = 4096, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    wrenSetSlotString(vm, 0, "Out of memory");
    wrenAbortFiber(vm, 0);
    return;
  }
  ssize_t n;
  while ((n = read(data->fd, buf + len, cap - len)) > 0) {
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

static void fileReadLine(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  size_t cap = 256, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    wrenSetSlotString(vm, 0, "Out of memory");
    wrenAbortFiber(vm, 0);
    return;
  }
  char c;
  while (read(data->fd, &c, 1) == 1) {
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

static void fileWrite(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  int length = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &length);
  ssize_t written = write(data->fd, s, (size_t)length);
  wrenSetSlotDouble(vm, 0, (double)(written < 0 ? 0 : written));
}

static void fileFlush(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  fsync(data->fd);
}

static void fileSeek(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  off_t off  = (off_t)wrenGetSlotDouble(vm, 1);
  int whence = (int)wrenGetSlotDouble(vm, 2);
  wrenSetSlotDouble(vm, 0, (double)lseek(data->fd, off, whence));
}

static void fileTell(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, (double)lseek(data->fd, 0, SEEK_CUR));
}

static void fileClose(WrenVM* vm) {
  FileData* data = (FileData*)wrenGetSlotForeign(vm, 0);
  if (data->fd >= 0) {
    close(data->fd);
    data->fd = -1;
  }
}

// ---- Module source --------------------------------------------------------

#include "fs.wren.inc"

const char* wrenFsSource() {
  return fsModuleSource;
}

// ---- Foreign method binding -----------------------------------------------

WrenForeignMethodFn wrenFsBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature)
{
  if (strcmp(className, "fs") == 0 && isStatic) {
    if (strcmp(signature, "exists_(_)") == 0)   return fsExists;
    if (strcmp(signature, "remove_(_)") == 0)   return fsRemove;
    if (strcmp(signature, "rename_(_,_)") == 0) return fsRename;
    if (strcmp(signature, "size_(_)") == 0)     return fsSize;
  }

  if (strcmp(className, "File") == 0) {
    if (isStatic) {
      if (strcmp(signature, "O_RDONLY") == 0)     return fileORdonly;
      if (strcmp(signature, "O_WRONLY") == 0)     return fileOWronly;
      if (strcmp(signature, "O_RDWR") == 0)       return fileORdwr;
      if (strcmp(signature, "O_CREATE") == 0)     return fileOCreate;
      if (strcmp(signature, "O_TRUNC") == 0)      return fileOTrunc;
      if (strcmp(signature, "O_APPEND") == 0)     return fileOAppend;
      if (strcmp(signature, "exists_(_)") == 0)   return fsExists;
      if (strcmp(signature, "remove_(_)") == 0)   return fsRemove;
      if (strcmp(signature, "rename_(_,_)") == 0) return fsRename;
      if (strcmp(signature, "size_(_)") == 0)     return fsSize;
    } else {
      if (strcmp(signature, "init new(_,_)") == 0) return fileInit;
      if (strcmp(signature, "fd") == 0)             return fileFd;
      if (strcmp(signature, "read(_)") == 0)        return fileRead;
      if (strcmp(signature, "readAll()") == 0)      return fileReadAll;
      if (strcmp(signature, "readLine()") == 0)     return fileReadLine;
      if (strcmp(signature, "write(_)") == 0)       return fileWrite;
      if (strcmp(signature, "flush()") == 0)        return fileFlush;
      if (strcmp(signature, "seek(_,_)") == 0)      return fileSeek;
      if (strcmp(signature, "tell()") == 0)         return fileTell;
      if (strcmp(signature, "close()") == 0)        return fileClose;
    }
  }

  return NULL;
}

// ---- Foreign class binding ------------------------------------------------

WrenForeignClassMethods wrenFsBindForeignClass(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* className)
{
  WrenForeignClassMethods methods = { NULL, NULL };

  if (strcmp(className, "File") == 0) {
    methods.allocate = fileAllocate;
    methods.finalize = fileFinalize;
  }

  return methods;
}
