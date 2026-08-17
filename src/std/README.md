# Pigeon Standard Library

Go-style modules with lowercase import paths and PascalCase types.

```
import "fs" for File
import "time" for Time, Duration
import "encoding/json" for Json
```

Registered modules:

| Import | Default export | Types / values |
|--------|----------------|----------------|
| `io` | `io` | `Reader`, `Writer`, `Pipe` |
| `fs` | `fs` | `File` |
| `os` | `os` | `Stdin`, `Stdout`, `Stderr` |
| `strconv` | `strconv` | `Atoi`, `Atof`, `Itoa`, `Ftoa` |
| `time` | `Time` | `Time`, `Duration`, `Timer` |
| `math` | `math` | `Stats`, constants |
| `math/random` | `Random` | `Random`, `RandomState` |
| `strings` | `strings` | `Builder` |
| `meta` | `meta` | `Meta` |
| `meta/reflection` | `reflection` | `ClassInfo`, `MethodInfo` |
| `encoding/json` | `Json` | `Json` |

Stdlib is also imported with a bare name (`"fs"`, not `"pigeon/fs"`).

## `io` — readers, writers, pipes

Transport-agnostic I/O over raw file descriptors.

```wren
import "io" for Pipe, Reader, Writer

var p = Pipe.create()
p.write("hello")
p.closeWrite()
System.print(p.readAll())  // hello
p.close()
```

Wrap an `os` or `fs.File` descriptor:

```wren
import "io" for io
import "os" for os
io.writer(os.stdout).writeln("hi")
```

## `fs` — files

`File` is the file type. Static helpers and instance methods share the same class.

```wren
import "fs" for File, fs

File.write("out.txt", "hello")
System.print(File.read("out.txt"))
System.print(fs.exists("out.txt"))

var f = File.new("out.txt", "r")
System.print(f.read(2))  // he
f.close()
```

Reads and writes are **byte-exact**. A Wren string is a byte buffer; embedded NUL
bytes are preserved. Do not treat file contents as C strings.

## `time`

```wren
import "time" for Time, Duration, Timer

var t = Time.new(2024, 1, 15, 10, 30, 0)
System.print(t.format("2006-01-02 15:04:05"))

var later = t.add(Duration.hours(2))
System.print(later.sub(t).seconds)  // 7200

Time.sleep(0.01)
```

## `encoding/json`

```wren
import "encoding/json" for Json

var obj = Json.parse("{\"a\":[1,true,null]}")
System.print(obj["a"][0])        // 1
System.print(Json.encode(obj))   // {"a":[1,true,null]}  (key order not stable)
```

`parse` is a C recursive-descent parser that builds `List` / `Map` through the
VM API. `encode` walks Wren values; object keys must be strings.

## `math/random`

```wren
import "math/random" for Random

var r = Random.new(42)   // deterministic
System.print(r.nextInt(100))
```

`Random.new()` without a seed uses the clock.

## Adding a module

1. `src/std/<name>/<name>.wren` plus `.c` / `.h` if there are foreign methods
2. `python3 util/wren_to_c_string.py src/std/<name>/<name>.wren.inc src/std/<name>/<name>.wren`
3. Register in `src/std/wren_stdlib.c` (name, source, binds, default export, export list)
4. Add the `.c` to `CMakeLists.txt`
5. Add `test/stdlib/<name>.wren`
