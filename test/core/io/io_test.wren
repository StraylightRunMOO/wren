// io comprehensive tests
import "io" for File

// Create a test file
var testPath = "/tmp/wren_io_test.txt"
var testContent = "Hello, Wren!"

// Test File.write
File.write(testPath, testContent)
System.print(File.exists(testPath) == true)  // expect: true

// Test File.read
var readContent = File.read(testPath)
System.print(readContent == testContent)  // expect: true

// Test File.size
System.print(File.size(testPath) == testContent.count)  // expect: true

// Test File.append
File.append(testPath, "\nLine 2")
var appended = File.read(testPath)
System.print(appended.contains("Line 2"))  // expect: true

// Test File.open with mode
var f = File.new(testPath, "r")
var partial = f.read(5)
System.print(partial == "Hello")  // expect: true
f.close()

// Test seek and tell
var f2 = File.new(testPath, "r")
System.print(f2.tell() == 0)  // expect: true
f2.seek(7, 0)  // SEEK_SET
System.print(f2.tell() == 7)  // expect: true
var afterSeek = f2.read(5)
System.print(afterSeek == "Wren!")  // expect: true
f2.close()

// Test File.rename
var newPath = "/tmp/wren_io_test_renamed.txt"
File.rename(testPath, newPath)
System.print(File.exists(testPath) == false)  // expect: true
System.print(File.exists(newPath) == true)  // expect: true

// Test File.remove
File.remove(newPath)
System.print(File.exists(newPath) == false)  // expect: true

// Test file flags constants
System.print(File.O_RDONLY == 0)  // expect: true
System.print(File.O_WRONLY == 1)  // expect: true
System.print(File.O_RDWR == 2)  // expect: true
System.print(File.O_CREATE == 64)  // expect: true
System.print(File.O_TRUNC == 512)  // expect: true
System.print(File.O_APPEND == 1024)  // expect: true

System.print("All io tests passed!")  // expect: All io tests passed!
