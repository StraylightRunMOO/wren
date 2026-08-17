import "fs" for File, fs

var path = "/tmp/pigeon_stdlib_fs.txt"
fs.write(path, "ok")
System.print(fs.exists(path))          // expect: true
System.print(File.read(path) == "ok")  // expect: true
File.remove(path)
System.print(fs.exists(path))          // expect: false
