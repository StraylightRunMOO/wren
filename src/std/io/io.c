#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "io.h"
#include "wren.h"
#include "wren_common.h"
#include "wren_vm.h"

// File handle management
#define MAX_FILES 256
static FILE* fileHandles[MAX_FILES] = {0};
static int nextHandle = 3;  // Start after stdin(0), stdout(1), stderr(2)

static int allocHandle(FILE* f) {
  if (!f) return -1;
  for (int i = 0; i < MAX_FILES; i++) {
    int h = (nextHandle + i) % MAX_FILES;
    if (h < 3) continue;  // Skip std streams
    if (!fileHandles[h]) {
      fileHandles[h] = f;
      nextHandle = h + 1;
      return h;
    }
  }
  fclose(f);
  return -1;  // Too many open files
}

static FILE* getHandle(int h) {
  if (h < 0 || h >= MAX_FILES) return NULL;
  return fileHandles[h];
}

static void freeHandle(int h) {
  if (h >= 3 && h < MAX_FILES) {
    fileHandles[h] = NULL;
  }
}

// Foreign method: File.open_(path, flags)
static void fileOpen(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  int flags = (int)wrenGetSlotDouble(vm, 2);
  
  const char* mode;
  if ((flags & O_RDWR)) {
    if (flags & O_TRUNC) mode = "w+";
    else if (flags & O_APPEND) mode = "a+";
    else mode = "r+";
  } else if (flags & O_WRONLY) {
    if (flags & O_APPEND) mode = "a";
    else mode = "w";
  } else {
    mode = "r";
  }
  
  FILE* f = fopen(path, mode);
  int h = allocHandle(f);
  wrenSetSlotDouble(vm, 0, h);
}

// Foreign method: File.exists_(path)
static void fileExists(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  wrenSetSlotBool(vm, 0, stat(path, &st) == 0);
}

// Foreign method: File.remove_(path)
static void fileRemove(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  wrenSetSlotBool(vm, 0, remove(path) == 0);
}

// Foreign method: File.rename_(old, new)
static void fileRename(WrenVM* vm) {
  const char* oldPath = wrenGetSlotString(vm, 1);
  const char* newPath = wrenGetSlotString(vm, 2);
  wrenSetSlotBool(vm, 0, rename(oldPath, newPath) == 0);
}

// Foreign method: File.size_(path)
static void fileSize(WrenVM* vm) {
  const char* path = wrenGetSlotString(vm, 1);
  struct stat st;
  if (stat(path, &st) == 0) {
    wrenSetSlotDouble(vm, 0, (double)st.st_size);
  } else {
    wrenSetSlotDouble(vm, 0, -1);
  }
}

// Foreign method: File.readAll_(handle)
static void fileReadAll(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  FILE* f = getHandle(h);
  if (!f) {
    wrenSetSlotString(vm, 0, "");
    return;
  }
  
  // Save position and seek to end
  long pos = ftell(f);
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  char* buf = malloc(size + 1);
  size_t n = fread(buf, 1, size, f);
  buf[n] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
  
  // Restore position if was reading
  if (pos >= 0) fseek(f, pos, SEEK_SET);
}

// Foreign method: File.read_(handle, bytes)
static void fileRead(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  int bytes = (int)wrenGetSlotDouble(vm, 2);
  FILE* f = getHandle(h);
  if (!f) {
    wrenSetSlotString(vm, 0, "");
    return;
  }
  
  char* buf = malloc(bytes + 1);
  size_t n = fread(buf, 1, bytes, f);
  buf[n] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
}

// Foreign method: File.write_(handle, data)
static void fileWrite(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  const char* data = wrenGetSlotString(vm, 2);
  FILE* f = getHandle(h);
  if (!f) {
    wrenSetSlotDouble(vm, 0, 0);
    return;
  }
  size_t n = fwrite(data, 1, strlen(data), f);
  wrenSetSlotDouble(vm, 0, (double)n);
}

// Foreign method: File.close_(handle)
static void fileClose(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  FILE* f = getHandle(h);
  if (f) {
    fclose(f);
    freeHandle(h);
  }
}

// Foreign method: File.seek_(handle, offset, whence)
static void fileSeek(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  long offset = (long)wrenGetSlotDouble(vm, 2);
  int whence = (int)wrenGetSlotDouble(vm, 3);
  FILE* f = getHandle(h);
  if (f) {
    fseek(f, offset, whence);
  }
}

// Foreign method: File.tell_(handle)
static void fileTell(WrenVM* vm) {
  int h = (int)wrenGetSlotDouble(vm, 1);
  FILE* f = getHandle(h);
  wrenSetSlotDouble(vm, 0, f ? (double)ftell(f) : -1);
}

// Foreign method: Stdin.read_()
static void stdinRead(WrenVM* vm) {
  char buf[4096];
  size_t n = fread(buf, 1, sizeof(buf)-1, stdin);
  buf[n] = '\0';
  wrenSetSlotString(vm, 0, buf);
}

// Foreign method: Stdin.readLine_()
static void stdinReadLine(WrenVM* vm) {
  char* line = NULL;
  size_t len = 0;
  ssize_t n = getline(&line, &len, stdin);
  if (n > 0 && line[n-1] == '\n') line[n-1] = '\0';
  wrenSetSlotString(vm, 0, line ? line : "");
  free(line);
}

// Foreign method: Stdout.write_(s)
static void stdoutWrite(WrenVM* vm) {
  const char* s = wrenGetSlotString(vm, 1);
  fwrite(s, 1, strlen(s), stdout);
  fflush(stdout);
}

// Foreign method: Stderr.write_(s)
static void stderrWrite(WrenVM* vm) {
  const char* s = wrenGetSlotString(vm, 1);
  fwrite(s, 1, strlen(s), stderr);
  fflush(stderr);
}

// Foreign method: Pipe.create_()
static void pipeCreate(WrenVM* vm) {
  int fds[2];
  if (pipe(fds) == -1) {
    // Error - abort the fiber
    wrenSetSlotString(vm, 0, "Failed to create pipe");
    return;
  }
  // Return read and write fds as a list
  wrenSetSlotNewList(vm, 0);
  wrenSetSlotDouble(vm, 1, fds[0]);
  wrenInsertInList(vm, 0, -1, 1);
  wrenSetSlotDouble(vm, 1, fds[1]);
  wrenInsertInList(vm, 0, -1, 1);
}

// Foreign method: Pipe.read_(fd, bytes)
static void pipeRead(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  int bytes = (int)wrenGetSlotDouble(vm, 2);
  
  char* buf = malloc(bytes + 1);
  ssize_t n = read(fd, buf, bytes);
  if (n < 0) {
    free(buf);
    wrenSetSlotString(vm, 0, "");
    return;
  }
  buf[n] = '\0';
  wrenSetSlotString(vm, 0, buf);
  free(buf);
}

// Foreign method: Pipe.write_(fd, data)
static void pipeWrite(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  const char* data = wrenGetSlotString(vm, 2);
  
  ssize_t n = write(fd, data, strlen(data));
  wrenSetSlotDouble(vm, 0, (double)(n < 0 ? 0 : n));
}

// Foreign method: Pipe.close_(fd)
static void pipeClose(WrenVM* vm) {
  int fd = (int)wrenGetSlotDouble(vm, 1);
  close(fd);
}

// Module source code (embedded from io.wren.inc)
#include "io.wren.inc"

const char* wrenIoSource() {
  return ioModuleSource;
}

WrenForeignMethodFn wrenIoBindForeignMethod(WrenVM* WREN_MAYBE_UNUSED vm,
                                            const char* className,
                                            bool isStatic,
                                            const char* signature)
{
  (void)vm;  // Unused but required by API
  
  // File class static methods
  if (strcmp(className, "File") == 0 && isStatic) {
    if (strcmp(signature, "open_(_,_)") == 0) return fileOpen;
    if (strcmp(signature, "exists_(_)") == 0) return fileExists;
    if (strcmp(signature, "remove_(_)") == 0) return fileRemove;
    if (strcmp(signature, "rename_(_,_)") == 0) return fileRename;
    if (strcmp(signature, "size_(_)") == 0) return fileSize;
    if (strcmp(signature, "readAll_(_)") == 0) return fileReadAll;
    if (strcmp(signature, "read_(_,_)") == 0) return fileRead;
    if (strcmp(signature, "write_(_,_)") == 0) return fileWrite;
    if (strcmp(signature, "close_(_)") == 0) return fileClose;
    if (strcmp(signature, "seek_(_,_,_)") == 0) return fileSeek;
    if (strcmp(signature, "tell_(_)") == 0) return fileTell;
  }
  
  // Stdin static methods
  if (strcmp(className, "Stdin") == 0 && isStatic) {
    if (strcmp(signature, "read_()") == 0) return stdinRead;
    if (strcmp(signature, "readLine_()") == 0) return stdinReadLine;
  }
  
  // Stdout static methods
  if (strcmp(className, "Stdout") == 0 && isStatic) {
    if (strcmp(signature, "write_(_)") == 0) return stdoutWrite;
  }
  
  // Stderr static methods
  if (strcmp(className, "Stderr") == 0 && isStatic) {
    if (strcmp(signature, "write_(_)") == 0) return stderrWrite;
  }
  
  // Pipe static methods (these are called as static methods on Pipe class)
  if (strcmp(className, "Pipe") == 0 && isStatic) {
    if (strcmp(signature, "create_()") == 0) return pipeCreate;
    if (strcmp(signature, "read_(_,_)") == 0) return pipeRead;
    if (strcmp(signature, "write_(_,_)") == 0) return pipeWrite;
    if (strcmp(signature, "close_(_)") == 0) return pipeClose;
  }
  
  return NULL;
}

WrenForeignClassMethods wrenIoBindForeignClass(WrenVM* WREN_MAYBE_UNUSED vm,
                                               const char* className)
{
  (void)vm;  // Unused but required by API
  
  WrenForeignClassMethods methods = { NULL, NULL };
  
  // File class is not a foreign class, it's pure Wren with static foreign methods
  (void)className;
  
  return methods;
}
