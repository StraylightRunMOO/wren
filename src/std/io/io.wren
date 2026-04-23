// io - Basic I/O operations for Wren
// Provides file operations, readers, writers, and utilities

class File {
  // Open flags
  static O_RDONLY { 0 }
  static O_WRONLY { 1 }
  static O_RDWR   { 2 }
  static O_CREATE { 64 }
  static O_TRUNC  { 512 }
  static O_APPEND { 1024 }

  construct new(path, mode) {
    _handle = File.open_(path, parseMode_(mode))
    if (_handle == -1) {
      Fiber.abort("Failed to open file: %(path)")
    }
  }

  static read(path) {
    var f = File.new(path, "r")
    var content = f.readAll()
    f.close()
    return content
  }

  static write(path, content) {
    var f = File.new(path, "w")
    f.write(content)
    f.close()
  }

  static append(path, content) {
    var f = File.new(path, "a")
    f.write(content)
    f.close()
  }

  static exists(path) { exists_(path) }
  static remove(path) { remove_(path) }
  static rename(old, new) { rename_(old, new) }
  static size(path) { size_(path) }

  readAll() { File.readAll_(_handle) }
  read(bytes) { File.read_(_handle, bytes) }
  write(data) { File.write_(_handle, data) }
  close() { 
    if (_handle != -1) {
      File.close_(_handle)
      _handle = -1
    }
  }

  seek(offset, whence) { File.seek_(_handle, offset, whence) }
  tell() { File.tell_(_handle) }

  parseMode_(mode) {
    if (mode == "r") return File.O_RDONLY
    if (mode == "w") return File.O_WRONLY | File.O_CREATE | File.O_TRUNC
    if (mode == "a") return File.O_WRONLY | File.O_CREATE | File.O_APPEND
    if (mode == "r+") return File.O_RDWR
    if (mode == "w+") return File.O_RDWR | File.O_CREATE | File.O_TRUNC
    if (mode == "a+") return File.O_RDWR | File.O_CREATE | File.O_APPEND
    Fiber.abort("Invalid file mode: %(mode)")
  }

  foreign static open_(path, flags)
  foreign static exists_(path)
  foreign static remove_(path)
  foreign static rename_(old, new)
  foreign static size_(path)
  foreign static readAll_(handle)
  foreign static read_(handle, bytes)
  foreign static write_(handle, data)
  foreign static close_(handle)
  foreign static seek_(handle, offset, whence)
  foreign static tell_(handle)
}

// Stdin, Stdout, Stderr as constants
class Stdin {
  static read() { read_() }
  static readLine() { readLine_() }
  foreign static read_()
  foreign static readLine_()
}

class Stdout {
  static write(s) { write_(s) }
  static writeln(s) { write_(s + "\n") }
  foreign static write_(s)
}

class Stderr {
  static write(s) { write_(s) }
  static writeln(s) { write_(s + "\n") }
  foreign static write_(s)
}

// Pipe for reading/writing
class Pipe {
  construct new(r, w) {
    _r = r
    _w = w
  }

  static create() {
    var fds = Pipe.create_()
    return Pipe.new(fds[0], fds[1])
  }

  read(bytes) { Pipe.read_(_r, bytes) }
  write(data) { Pipe.write_(_w, data) }
  closeRead() { Pipe.close_(_r) }
  closeWrite() { Pipe.close_(_w) }
  close() {
    closeRead()
    closeWrite()
  }

  foreign static create_()
  foreign static read_(fd, bytes)
  foreign static write_(fd, data)
  foreign static close_(fd)
}
