// io - Transport-agnostic I/O primitives
// Reader, Writer, and Pipe work over raw file descriptors.
// Compatible with any fd source: fs.File, os.Stdin/Stdout, network sockets.

class io {
  static reader(fd) { Reader.new(fd) }
  static writer(fd) { Writer.new(fd) }
  static pipe()     { Pipe.create() }
}

class Reader {
  construct new(fd) { _fd = fd }

  fd { _fd }

  read(n)    { Reader.read_(_fd, n) }
  readAll()  { Reader.readAll_(_fd) }
  readLine() { Reader.readLine_(_fd) }
  close()    { Reader.close_(_fd) }

  foreign static read_(fd, n)
  foreign static readAll_(fd)
  foreign static readLine_(fd)
  foreign static close_(fd)
}

class Writer {
  construct new(fd) { _fd = fd }

  fd { _fd }

  write(s)   { Writer.write_(_fd, s) }
  writeln(s) { Writer.write_(_fd, s + "\n") }
  flush()    { Writer.flush_(_fd) }
  close()    { Writer.close_(_fd) }

  foreign static write_(fd, s)
  foreign static flush_(fd)
  foreign static close_(fd)
}

class Pipe {
  construct new(rfd, wfd) {
    _reader = Reader.new(rfd)
    _writer = Writer.new(wfd)
  }

  static create() {
    var fds = Pipe.create_()
    return Pipe.new(fds[0], fds[1])
  }

  reader { _reader }
  writer { _writer }

  read(n)    { _reader.read(n) }
  readAll()  { _reader.readAll() }
  readLine() { _reader.readLine() }
  write(s)   { _writer.write(s) }

  closeRead()  { _reader.close() }
  closeWrite() { _writer.close() }
  close() {
    closeRead()
    closeWrite()
  }

  foreign static create_()
}
