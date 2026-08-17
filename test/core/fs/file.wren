// File lives in "fs". Static helpers + instance read/seek/rename.
import "fs" for File

var testPath = "/tmp/pigeon_io_test.txt"
var testContent = "Hello, Wren!"

File.write(testPath, testContent)
System.print(File.exists(testPath) == true)  // expect: true

var readContent = File.read(testPath)
System.print(readContent == testContent)  // expect: true

System.print(File.size(testPath) == testContent.count)  // expect: true

File.append(testPath, "\nLine 2")
var appended = File.read(testPath)
System.print(appended.contains("Line 2"))  // expect: true

var f = File.new(testPath, "r")
var partial = f.read(5)
System.print(partial == "Hello")  // expect: true
f.close()

var f2 = File.new(testPath, "r")
System.print(f2.tell() == 0)  // expect: true
f2.seek(7, 0)
System.print(f2.tell() == 7)  // expect: true
var afterSeek = f2.read(5)
System.print(afterSeek == "Wren!")  // expect: true
f2.close()

var newPath = "/tmp/pigeon_io_test_renamed.txt"
File.rename(testPath, newPath)
System.print(File.exists(testPath) == false)  // expect: true
System.print(File.exists(newPath) == true)  // expect: true

File.remove(newPath)
System.print(File.exists(newPath) == false)  // expect: true

System.print(File.O_RDONLY == 0)  // expect: true
System.print(File.O_WRONLY == 1)  // expect: true
System.print(File.O_RDWR == 2)  // expect: true
System.print(File.O_CREATE == 64)  // expect: true
System.print(File.O_TRUNC == 512)  // expect: true
System.print(File.O_APPEND == 1024)  // expect: true
