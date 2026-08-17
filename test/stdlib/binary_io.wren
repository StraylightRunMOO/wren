import "fs" for File

var path = "/tmp/pigeon_binary_io.bin"
var payload = "X" + String.fromByte(0) + "Y" + String.fromByte(255)
File.write(path, payload)
var got = File.read(path)
System.print(got.bytes.count == 4)  // expect: true
System.print(got.bytes[0] == 88)    // expect: true
System.print(got.bytes[1] == 0)     // expect: true
System.print(got.bytes[2] == 89)    // expect: true
System.print(got.bytes[3] == 255)   // expect: true
File.remove(path)
