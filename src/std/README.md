# Wren Standard Library

A Go-style standard library for Wren with hierarchical modules.

## Structure

```
src/std/
├── wren_stdlib.h       # Main stdlib header
├── wren_stdlib.c       # Module registry
├── io/                 # File I/O operations
├── strconv/            # String conversions
├── time/               # Time and date
├── math/               # Mathematical functions
├── strings/            # String utilities
└── README.md           # This file
```

## Modules

### `io` - File I/O Operations

Basic file operations, stdin/stdout/stderr access.

```wren
import "io"

// File operations
var content = io.File.read("file.txt")
io.File.write("output.txt", "Hello, World!")
io.File.append("log.txt", "New entry\n")

// File info
if (io.File.exists("file.txt")) {
  System.print("Size: %(io.File.size("file.txt"))")
}

// Working with files
var f = io.File.new("data.txt", "r")
var data = f.readAll()
f.close()

// Standard streams
io.Stdout.write("Hello")
io.Stderr.writeln("Error message")
var line = io.Stdin.readLine()
```

### `strconv` - String Conversions

Parse and format numbers, quote strings.

```wren
import "strconv"

// Parse strings to numbers
var n = strconv.parseInt("42")
var f = strconv.parseFloat("3.14")
var hex = strconv.parseInt("FF", 16)

// Format numbers as strings
var s = strconv.formatInt(42)           // "42"
var h = strconv.formatInt(255, 16)      // "ff"
var pi = strconv.formatFloat(3.14159)   // "3.14159"

// Quote strings
var quoted = strconv.quote("hello\nworld")  // "hello\nworld"

// Check validity
if (strconv.isInt("123")) {
  // Valid integer
}
```

### `time` - Time and Date

Time operations, formatting, durations.

```wren
import "time"

// Current time
var now = time.Time.now()
var ms = time.Time.nowMillis()

// Create time from components
var t = time.Time.new(2024, 1, 15, 10, 30, 0)

// Format time
System.print(t.format("2006-01-02 15:04:05"))

// Durations
var d = time.Duration.hours(2)
var later = t.add(d)
var elapsed = later.sub(t)

// Timer
var timer = time.Timer.new().start()
// ... do work ...
var elapsed = timer.stop().elapsed

// Sleep
time.Time.sleep(0.5)  // Sleep for 0.5 seconds
```

### `math` - Mathematical Functions

Extended mathematical operations.

```wren
import "math"

// Constants
System.print(math.Pi)
System.print(math.E)

// Basic functions
var a = math.abs(-5)
var c = math.ceil(3.2)
var f = math.floor(3.8)

// Trigonometry
var s = math.sin(math.Pi / 2)
var angle = math.atan2(1, 1)

// Special functions
var g = math.gamma(5)

// Statistics
var sum = math.Stats.sum([1, 2, 3, 4, 5])
var avg = math.Stats.mean([1, 2, 3, 4, 5])

// Random
var r = math.Rand.new()
var n = r.int(100)      // 0 to 99
var x = r.range(0, 1)   // 0.0 to 1.0
```

### `strings` - String Utilities

String manipulation functions (Go-style).

```wren
import "strings"

// Check prefixes/suffixes
if (strings.hasPrefix("hello.txt", "hello")) {
  // ...
}

// Split and join
var parts = strings.split("a,b,c", ",")
var joined = strings.join(parts, "-")

// Replace
var result = strings.replace("hello world", "world", "wren")

// Trim
var clean = strings.trim("  hello  ")
var stripped = strings.trimPrefix("prefix_text", "prefix_")

// Case conversion
var upper = strings.toUpper("hello")
var lower = strings.toLower("WORLD")

// String builder
var b = strings.Builder.new()
b.write("Hello").write(" ").writeln("World")
System.print(b.toString)
```

## Adding New Modules

1. Create a new directory: `mkdir src/std/mymodule`

2. Create the files:
   - `mymodule.wren` - Wren source code
   - `mymodule.h` - C header
   - `mymodule.c` - C implementation

3. Generate the `.wren.inc` file:
   ```bash
   python3 util/wren_to_c_string.py src/std/mymodule/mymodule.wren.inc src/std/mymodule/mymodule.wren
   ```

4. Register in `src/std/wren_stdlib.c`:
   ```c
   #include "mymodule/mymodule.h"
   
   static StdlibModule stdlibModules[] = {
     // ... existing modules ...
     { "mymodule", wrenMymoduleSource, wrenMymoduleBindForeignMethod },
     { NULL, NULL, NULL }
   };
   ```

5. Update `CMakeLists.txt` to include the new source files.

## Module Naming

Following Go conventions:
- Short, lowercase names
- No underscores (except for subpackages)
- Clear, single-purpose modules

Future modules could include:
- `net/http` - HTTP client/server
- `encoding/json` - JSON encoding/decoding
- `crypto/md5` - Cryptographic functions
- `compress/gzip` - Compression
- `sync` - Synchronization primitives
