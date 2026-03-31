## Wren is a small, fast, class-based concurrent scripting language

Think Smalltalk in a Lua-sized package with a dash of Erlang and wrapped up in
a familiar, modern [syntax][syntax].

```dart
System.print("Hello, world!")

class Wren {
  flyTo(city) {
    System.print("Flying to %(city)")
  }
}

var adjectives = Fiber.new {
  ["small", "clean", "fast"].each {|word| Fiber.yield(word) }
}

while (!adjectives.isDone) System.print(adjectives.call())
```

 *  **Wren is small.** The VM implementation is under [4,000 semicolons][src].
    You can skim the whole thing in an afternoon. It's *small*, but not
    *dense*. It is readable and [lovingly-commented][nan].

 *  **Wren is fast.** A fast single-pass compiler to tight bytecode, and a
    compact object representation help Wren [compete with other dynamic
    languages][perf].

 *  **Wren is class-based.** There are lots of scripting languages out there,
    but many have unusual or non-existent object models. Wren places
    [classes][] front and center.

 *  **Wren is concurrent.** Lightweight [fibers][] are core to the execution
    model and let you organize your program into an army of communicating
    coroutines.

 *  **Wren is a scripting language.** Wren is intended for embedding in
    applications. It has no dependencies, a small standard library,
    and [an easy-to-use C API][embedding]. It compiles cleanly as C99, C++98
    or anything later.

## Quick Start

### Building

Wren uses CMake for building. You'll need CMake 3.15 or later.

```bash
# Configure the build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build -j$(nproc)
```

This creates:
- `build/lib/libwren.a` - Static library
- `build/lib/libwren.so` - Shared library (on Unix)
- `build/bin/wren_test` - Test runner
- `build/bin/wren_repl` - Interactive REPL

For more build options, see [BUILD.md](BUILD.md).

### Running

```bash
# Run the REPL
./build/bin/wren_repl

# Run a Wren file
./build/bin/wren_test my_script.wren
```

### Testing

```bash
# Run all tests
python3 util/test.py

# Run a specific test suite
python3 util/test.py language
python3 util/test.py core
python3 util/test.py api

# Run a specific test file
python3 util/test.py language/function
```

### Benchmarks

```bash
# Run benchmarks
./build/bin/wren_test test/benchmark/binary_trees.wren
./build/bin/wren_test test/benchmark/fib.wren
./build/bin/wren_test test/benchmark/delta_blue.wren
```

## New Features

### Anonymous Functions with `fn`

Wren now uses the `fn` keyword for anonymous functions:

```dart
// Basic anonymous function
var greet = fn (name) { "Hello, %(name)!" }
System.print(greet("Wren"))  // Hello, Wren!

// Shorthand for no parameters
var getNum = fn { 42 }
System.print(getNum())  // 42

// Used with higher-order functions
var doubled = [1, 2, 3].map(fn (n) { n * 2 })
System.print(doubled)  // [2, 4, 6]
```

### Method Call Without Parentheses

Methods that take no arguments can be called without parentheses:

```dart
class Counter {
  construct new() { _count = 0 }
  increment { _count = _count + 1 }
  value { _count }
}

var counter = Counter.new
counter.increment  // Same as counter.increment()
System.print(counter.value)  // Same as counter.value()
```

### 0 is Falsy

The number `0` is now falsy in boolean contexts:

```dart
System.print(0 || "default")    // default
System.print(0 && "yes")        // 0
System.print(1 || "default")    // 1
```

### Object Number Literals

Embed foreign objects using `#` syntax with a C callback:

```c
// In your C code
WrenConfiguration config;
wrenInitConfiguration(&config);
config.objectNumberFn = [](WrenVM* vm, int64_t value) {
    // Create and return your object
    wrenSetSlotDouble(vm, 0, value * 2);
};
```

```dart
// In Wren
var doubled = #21   // Calls your callback with 21
System.print(doubled)  // 42
```

## Documentation

- [Language Syntax][syntax]
- [Embedding Guide][embedding]
- [Concurrency (Fibers)][fibers]
- [Classes][classes]
- [Performance][perf]

## Getting Involved

- Official website: http://wren.io/
- GitHub: https://github.com/wren-lang/wren
- Try Wren in your browser: http://ppvk.github.io/wren-nest/

If you like the sound of this, [let's get started][started]. Excited? Well, come on and [get
involved][contribute]!

[syntax]: http://wren.io/syntax.html
[src]: https://github.com/wren-lang/wren/tree/main/src
[nan]: https://github.com/wren-lang/wren/blob/93dac9132773c5bc0bbe92df5ccbff14da9d25a6/src/vm/wren_value.h#L486-L541
[perf]: http://wren.io/performance.html
[classes]: http://wren.io/classes.html
[fibers]: http://wren.io/concurrency.html
[embedding]: http://wren.io/embedding/
[started]: http://wren.io/getting-started.html
[contribute]: http://wren.io/contributing.html
