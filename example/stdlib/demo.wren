// Pigeon standard library demo

import "fs" for File
import "time" for Time, Duration
import "strings" for strings
import "math" for math
import "math/random" for Random
import "encoding/json" for Json
import "strconv" for strconv

File.write("demo_out.txt", "Hello, Pigeon.\n")
File.append("demo_out.txt", "Line two.\n")
System.print(File.read("demo_out.txt"))
System.print("size %(File.size("demo_out.txt"))")
File.remove("demo_out.txt")

System.print("parse 42 -> %(strconv.parseInt("42"))")
System.print("now %(Time.now())")
var t = Time.new(2024, 1, 15, 10, 30, 0)
System.print(t.format("2006-01-02 15:04:05"))
System.print("2h = %(Duration.hours(2).seconds)s")

System.print(strings.toUpper("hello"))
System.print(math.sin(math.Pi / 2))

var r = Random.new(1)
System.print("rand %(r.nextInt(100))")

System.print(Json.encode({"ok": true, "n": 3}))
