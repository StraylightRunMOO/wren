// io is transport-agnostic: Reader, Writer, Pipe over raw fds.
import "io" for Pipe

var p = Pipe.create()
p.write("Hello")
p.closeWrite()
System.print(p.read(5) == "Hello")  // expect: true
p.close()

// Binary-safe: a NUL in the middle is preserved.
var p2 = Pipe.create()
var payload = "A" + String.fromByte(0) + "B"
p2.write(payload)
p2.closeWrite()
var got = p2.readAll()
System.print(got.bytes.count == 3)  // expect: true
System.print(got.bytes[0] == 65)    // expect: true
System.print(got.bytes[1] == 0)     // expect: true
System.print(got.bytes[2] == 66)    // expect: true
p2.close()

System.print("All io tests passed!")  // expect: All io tests passed!
