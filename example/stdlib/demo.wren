// Wren Standard Library Demo
// Shows usage of the Go-style stdlib modules

// File I/O operations
import "io"

// Write a file
io.File.write("test.txt", "Hello, Wren Standard Library!\n")

// Append to file
io.File.append("test.txt", "This is a new line.\n")

// Read it back
var content = io.File.read("test.txt")
System.print("File contents:")
System.print(content)

// Check if file exists
if (io.File.exists("test.txt")) {
  System.print("File size: %(io.File.size("test.txt")) bytes")
}

// Clean up
io.File.remove("test.txt")

// String conversions
import "strconv"

System.print("\n--- strconv ---")
var n = strconv.parseInt("42")
var f = strconv.parseFloat("3.14159")
var hex = strconv.parseInt("FF", 16)

System.print("Parsed 42: %(n)")
System.print("Parsed 3.14159: %(f)")
System.print("Parsed FF (hex): %(hex)")

System.print("Format 255 as hex: %(strconv.formatInt(255, 16))")
System.print("Format Pi: %(strconv.formatFloat(3.14159))")

// Time operations
import "time"

System.print("\n--- time ---")
var now = time.Time.now()
System.print("Current time (epoch): %(now)")

var t = time.Time.new(2024, 1, 15, 10, 30, 0)
System.print("Created time: %(t.format("2006-01-02 15:04:05"))")

var d = time.Duration.hours(2)
System.print("2 hours in seconds: %(d.seconds)")

// String utilities
import "strings"

System.print("\n--- strings ---")
var s = "hello, world"
System.print("Has prefix 'hello': %(strings.hasPrefix(s, "hello"))")
System.print("To upper: %(strings.toUpper(s))")

var parts = strings.split("a,b,c,d", ",")
System.print("Split result: %(parts)")

var joined = strings.join(parts, "-")
System.print("Joined: %(joined)")

// Math operations
import "math"

System.print("\n--- math ---")
System.print("Pi: %(math.Pi)")
System.print("E: %(math.E)")
System.print("sqrt(2): %(math.Sqrt2)")

System.print("sin(Pi/2): %(math.sin(math.Pi / 2))")
System.print("clamp(10, 0, 5): %(math.clamp(10, 0, 5))")

// Random numbers
var r = math.Rand.new()
System.print("Random int (0-99): %(r.int(100))")
System.print("Random float (0-1): %(r.float())")

// Statistics
var data = [3, 1, 4, 1, 5, 9, 2, 6]
System.print("Data: %(data)")
System.print("Sum: %(math.Stats.sum(data))")
System.print("Mean: %(math.Stats.mean(data))")
System.print("Min: %(math.Stats.min(data))")
System.print("Max: %(math.Stats.max(data))")

System.print("\nDemo complete!")
