// fs - Filesystem operations
// fs.open() returns a File, which implements the Reader/Writer protocol over a raw fd.

class fs {
  static open(path, mode)   { File.new(path, mode) }
  static read(path)         {
    var f = File.new(path, "r")
    var s = f.readAll()
    f.close()
    return s
  }
  static write(path, data)  {
    var f = File.new(path, "w")
    f.write(data)
    f.close()
  }
  static append(path, data) {
    var f = File.new(path, "a")
    f.write(data)
    f.close()
  }
  static exists(path)       { fs.exists_(path) }
  static remove(path)       { fs.remove_(path) }
  static rename(old, new)   { fs.rename_(old, new) }
  static size(path)         { fs.size_(path) }

  foreign static exists_(path)
  foreign static remove_(path)
  foreign static rename_(old, new)
  foreign static size_(path)
}

// File wraps a raw fd and implements the Reader/Writer protocol.
// Types (File, etc.) stay PascalCase.
class File {
  static O_RDONLY { 0 }
  static O_WRONLY { 1 }
  static O_RDWR   { 2 }
  static O_CREATE { 64 }
  static O_TRUNC  { 512 }
  static O_APPEND { 1024 }

  construct new(path, mode) {
    _fd = File.open_(path, parseMode_(mode))
    if (_fd == -1) Fiber.abort("Failed to open file: %(path)")
  }

  // Raw fd — can be passed to io.reader() / io.writer()
  fd { _fd }

  read(n)    { File.read_(_fd, n) }
  readAll()  { File.readAll_(_fd) }
  readLine() { File.readLine_(_fd) }
  write(s)   { File.write_(_fd, s) }
  writeln(s) { File.write_(_fd, s + "\n") }
  flush()    { File.flush_(_fd) }
  seek(offset, whence) { File.seek_(_fd, offset, whence) }
  tell()     { File.tell_(_fd) }
  close() {
    if (_fd != -1) {
      File.close_(_fd)
      _fd = -1
    }
  }

  parseMode_(mode) {
    if (mode == "r")  return File.O_RDONLY
    if (mode == "w")  return File.O_WRONLY | File.O_CREATE | File.O_TRUNC
    if (mode == "a")  return File.O_WRONLY | File.O_CREATE | File.O_APPEND
    if (mode == "r+") return File.O_RDWR
    if (mode == "w+") return File.O_RDWR | File.O_CREATE | File.O_TRUNC
    if (mode == "a+") return File.O_RDWR | File.O_CREATE | File.O_APPEND
    Fiber.abort("Invalid file mode: %(mode)")
  }

  foreign static open_(path, flags)
  foreign static read_(fd, n)
  foreign static readAll_(fd)
  foreign static readLine_(fd)
  foreign static write_(fd, s)
  foreign static flush_(fd)
  foreign static seek_(fd, offset, whence)
  foreign static tell_(fd)
  foreign static close_(fd)
}
