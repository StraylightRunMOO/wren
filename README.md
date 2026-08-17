## Pigeon is a small, fast, class-based concurrent scripting language

Pigeon is a fork of [Wren](https://wren.io) — think Smalltalk in a Lua-sized
package, with fibers for concurrency and a familiar, modern
[syntax][syntax]. This tree is **not** upstream Wren: the public name, C API,
and CLI are Pigeon 0.5.

```dart
System.print("Hello, world!")

class Pigeon {
  flyTo(city) {
    System.print("Flying to %(city)")
  }
}

var adjectives = Fiber.new {
  ["small", "clean", "fast"].each {|word| Fiber.yield(word) }
}

while (!adjectives.isDone) System.print(adjectives.call())
```

 *  **Pigeon is small.** The VM is a compact, readable C11 bytecode interpreter.
 *  **Pigeon is fast.** A single-pass compiler, NaN-tagged values, numeric
    opcodes, and a monomorphic call cache keep it competitive with other
    dynamic languages.
 *  **Pigeon is class-based.** [Classes][] sit at the center of the object model.
 *  **Pigeon is concurrent.** Lightweight [fibers][] are core to execution.
 *  **Pigeon is a scripting language.** Embed it with [pigeon.h][embedding].
    `wren.h` is a one-release compatibility shim.

Derived from Wren (MIT, Robert Nystrom and [contributors](AUTHORS)).

## Quick Start

### Building

CMake 3.15+, a C11 compiler, and (on Linux) `liburing`.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

This creates:
- `build/lib/libpigeon.a` — static library
- `build/lib/libpigeon.so` — shared library (Unix)
- `build/bin/pigeon` — CLI (REPL, or run a file)
- `build/bin/pigeon_test` — language / API test harness

See [BUILD.md](BUILD.md) for options (`WREN_NAN_TAGGING`, sibling Memento /
Suspenders trees, etc.).

### Running

```bash
# Interactive REPL
./build/bin/pigeon

# Run a source file
./build/bin/pigeon example/hello.wren

# Version
./build/bin/pigeon --version
```

### Embedding

```c
#include "pigeon.h"

int main(void) {
  PigeonVM* vm = pigeonNewVM(NULL);
  pigeonInterpret(vm, "main", "System.print(\"hi\")");
  pigeonFreeVM(vm);
  return 0;
}
```

Existing hosts can keep `#include "wren.h"` for this release; it maps `WrenVM`
/ `wrenInterpret` onto the Pigeon names and emits a deprecation warning.

### Testing

```bash
python3 util/test.py
python3 util/test.py language
python3 util/test.py core
```

[syntax]: http://wren.io/syntax.html
[classes]: http://wren.io/classes.html
[fibers]: http://wren.io/concurrency.html
[embedding]: doc/site/embedding/
