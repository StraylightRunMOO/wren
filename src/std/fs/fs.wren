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
  static exists(path)       { exists_(path) }
  static remove(path)       { remove_(path) }
  static rename(old, new)   { rename_(old, new) }
  static size(path)         { size_(path) }

  foreign static exists_(path)
  foreign static remove_(path)
  foreign static rename_(old, new)
  foreign static size_(path)
}

// File wraps a raw fd and implements the Reader/Writer protocol.
// Foreign class — the fd is stored in C-allocated memory so the GC
// can close it via the finalizer if the user forgets.
foreign class File {
  foreign static O_RDONLY
  foreign static O_WRONLY
  foreign static O_RDWR
  foreign static O_CREATE
  foreign static O_TRUNC
  foreign static O_APPEND

  static read(path) {
    var f = new(path, "r")
    var s = f.readAll()
    f.close()
    return s
  }
  static write(path, data) {
    var f = new(path, "w")
    f.write(data)
    f.close()
  }
  static append(path, data) {
    var f = new(path, "a")
    f.write(data)
    f.close()
  }
  static exists(path) { exists_(path) }
  static remove(path) { remove_(path) }
  static rename(old, new) { rename_(old, new) }
  static size(path) { size_(path) }

  foreign static exists_(path)
  foreign static remove_(path)
  foreign static rename_(old, new)
  foreign static size_(path)

  construct new(path, mode) {}

  foreign fd

  foreign read(n)
  foreign readAll()
  foreign readLine()
  foreign write(s)
  foreign flush()
  foreign seek(offset, whence)
  foreign tell()
  foreign close()

  writeln(s) { write(s + "\n") }
}
