import "io" for io, Pipe

var p = io.pipe()
p.writer.write("hi")
p.closeWrite()
System.print(p.reader.readAll() == "hi")  // expect: true
p.close()
