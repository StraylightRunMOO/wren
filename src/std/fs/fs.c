#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "fs.h"
#include "pigeon.h"
#include "wren_common.h"

// Foreign data for File instances.
typedef struct {
  int fd;
} FileData;

// ---- fs static methods (filesystem operations, no File instance) ----------

static void fsExists(PigeonVM* vm) {
  const char* path = pigeonGetSlotString(vm, 1);
  struct stat st;
  pigeonSetSlotBool(vm, 0, stat(path, &st) == 0);
}

static void fsRemove(PigeonVM* vm) {
  const char* path = pigeonGetSlotString(vm, 1);
  pigeonSetSlotBool(vm, 0, remove(path) == 0);
}

static void fsRename(PigeonVM* vm) {
  const char* oldPath = pigeonGetSlotString(vm, 1);
  const char* newPath = pigeonGetSlotString(vm, 2);
  pigeonSetSlotBool(vm, 0, rename(oldPath, newPath) == 0);
}

static void fsSize(PigeonVM* vm) {
  const char* path = pigeonGetSlotString(vm, 1);
  struct stat st;
  pigeonSetSlotDouble(vm, 0, stat(path, &st) == 0 ? (double)st.st_size : -1);
}

// ---- File foreign class ---------------------------------------------------

static void fileAllocate(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonSetSlotNewForeign(vm, 0, 0, sizeof(FileData));
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

static void fileORdonly(PigeonVM* vm) { pigeonSetSlotDouble(vm, 0, O_RDONLY); }
static void fileOWronly(PigeonVM* vm) { pigeonSetSlotDouble(vm, 0, O_WRONLY); }
static void fileORdwr(PigeonVM* vm)   { pigeonSetSlotDouble(vm, 0, O_RDWR); }
static void fileOCreate(PigeonVM* vm) { pigeonSetSlotDouble(vm, 0, O_CREAT); }
static void fileOTrunc(PigeonVM* vm)  { pigeonSetSlotDouble(vm, 0, O_TRUNC); }
static void fileOAppend(PigeonVM* vm) { pigeonSetSlotDouble(vm, 0, O_APPEND); }

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
static void fileInit(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  const char* path = pigeonGetSlotString(vm, 1);
  const char* mode = pigeonGetSlotString(vm, 2);

  int flags = parseMode(mode);
  if (flags == -1) {
    pigeonSetSlotString(vm, 0, "Invalid file mode");
    pigeonAbortFiber(vm, 0);
    return;
  }

  data->fd = open(path, flags, 0644);
  if (data->fd < 0) {
    pigeonSetSlotString(vm, 0, "Failed to open file");
    pigeonAbortFiber(vm, 0);
  }
}

// ---- File instance methods ------------------------------------------------

static void fileFd(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  pigeonSetSlotDouble(vm, 0, data->fd);
}

static void fileRead(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  int n = (int)pigeonGetSlotDouble(vm, 1);
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
  ssize_t got = read(data->fd, buf, (size_t)n);
  if (got < 0) got = 0;
  pigeonSetSlotBytes(vm, 0, buf, (size_t)got);
  free(buf);
}

static void fileReadAll(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  size_t cap = 4096, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
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

static void fileReadLine(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  size_t cap = 256, len = 0;
  char* buf = malloc(cap);
  if (buf == NULL) {
    pigeonSetSlotString(vm, 0, "Out of memory");
    pigeonAbortFiber(vm, 0);
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

static void fileWrite(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  int length = 0;
  const char* s = pigeonGetSlotBytes(vm, 1, &length);
  ssize_t written = write(data->fd, s, (size_t)length);
  pigeonSetSlotDouble(vm, 0, (double)(written < 0 ? 0 : written));
}

static void fileFlush(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  fsync(data->fd);
}

static void fileSeek(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  off_t off  = (off_t)pigeonGetSlotDouble(vm, 1);
  int whence = (int)pigeonGetSlotDouble(vm, 2);
  pigeonSetSlotDouble(vm, 0, (double)lseek(data->fd, off, whence));
}

static void fileTell(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  pigeonSetSlotDouble(vm, 0, (double)lseek(data->fd, 0, SEEK_CUR));
}

static void fileClose(PigeonVM* vm) {
  FileData* data = (FileData*)pigeonGetSlotForeign(vm, 0);
  if (data->fd >= 0) {
    close(data->fd);
    data->fd = -1;
  }
}

// ---- Module source --------------------------------------------------------

#include "fs.wren.inc"

const char* pigeonFsSource() {
  return fsModuleSource;
}

// ---- Foreign method binding -----------------------------------------------

PigeonForeignMethodFn pigeonFsBindForeignMethod(PigeonVM* PIGEON_MAYBE_UNUSED vm,
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

PigeonForeignClassMethods pigeonFsBindForeignClass(PigeonVM* PIGEON_MAYBE_UNUSED vm,
                                               const char* className)
{
  PigeonForeignClassMethods methods = { NULL, NULL };

  if (strcmp(className, "File") == 0) {
    methods.allocate = fileAllocate;
    methods.finalize = fileFinalize;
  }

  return methods;
}
