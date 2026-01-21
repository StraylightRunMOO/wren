# Wren Tools

This directory contains useful tools for working with Wren.

## wren_repl - Interactive REPL

A full-featured Read-Eval-Print Loop for experimenting with Wren code interactively.

### Building

The REPL is built automatically when you build the Wren project with CMake:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

The REPL binary will be located at `build/bin/wren_repl`.

### Usage

#### Interactive Mode

Simply run the REPL:

```bash
./build/bin/wren_repl
```

You'll see a welcome banner and prompt where you can enter Wren expressions and statements:

```
wren> 2 + 2
4

wren> var x = 10

wren> x * 5
50

wren> System.print("Hello!")
Hello!

wren> [1, 2, 3].map {|n| n * 2 }
```

#### Execute a File Then Enter REPL

You can execute a Wren file and then enter the REPL with that context:

```bash
./build/bin/wren_repl my_script.wren
```

### Commands

The REPL supports several dot commands:

- `.help` - Show help message with all available commands
- `.quit` / `.exit` / `.q` - Exit the REPL
- `.clear` / `.cls` - Clear the screen
- `.reset` - Reset the VM and clear all state
- `.vars` - Show defined variables (not yet implemented)

### Features

**Expression Auto-Print**: Simple expressions without semicolons are automatically printed:
```
wren> 10 + 20
30
```

**Multi-line Input**: The REPL automatically detects incomplete blocks and allows multi-line input:
```
wren> class Point {
....>   construct new(x, y) {
....>     _x = x
....>     _y = y
....>   }
....> }
```

**Syntax Highlighting**: Output uses ANSI colors for better readability:
- Cyan prompts
- Green output
- Red errors
- Gray info messages

**Command History**: Previous commands are stored (up to 100 entries)

**Statement Support**: Both statements and expressions work:
```
wren> var x = 5         // Statement
wren> x + 10            // Expression - prints 15
wren> if (x > 3) System.print("yes")   // Statement
yes
```

### Tips

- Type expressions without semicolons to see their results
- Use curly braces `{}` for multi-line code blocks
- Use `.reset` to start fresh without restarting the REPL
- Press Ctrl+C (or Ctrl+D) to exit

### Examples

```bash
# Basic arithmetic
wren> 2 + 2 * 3
8

# Variables
wren> var name = "Wren"
wren> "Hello, " + name + "!"
Hello, Wren!

# Lists and functional operations
wren> var nums = [1, 2, 3, 4, 5]
wren> nums.map {|n| n * n }

# Classes
wren> class Greeter {
....>   construct new(name) { _name = name }
....>   greet() { System.print("Hello, " + _name) }
....> }
wren> var g = Greeter.new("World")
wren> g.greet()
Hello, World
```

### Future Enhancements

Potential improvements for the REPL:

- [ ] Variable inspection (`.vars` command)
- [ ] Command history with up/down arrow keys
- [ ] Tab completion for identifiers
- [ ] Syntax highlighting for input
- [ ] Save/load session state
- [ ] Better multi-line editing
- [ ] Help command with examples
