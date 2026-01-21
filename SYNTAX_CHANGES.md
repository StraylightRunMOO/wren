# Wren Function Syntax Changes

Based on analysis of commits from the functional branch of StraylightRunMOO/wren:

## Commit a45a436: "Add an anonymous expression fn syntax"

This commit introduced new syntax for functions.

### 1. Anonymous Function Declaration

**Before:**
```wren
var f = Fn.new { 123 }
var f = Fn.new {|a, b| a + b}
```

**After:**
```wren
var f = fn () { 123 }
var f = fn (a, b) { a + b }
```

### 2. Function Calls

**Both syntaxes work (with .call() being the old way):**
```wren
fn.call(1, 2)  // OLD syntax
fn(1, 2)       // NEW syntactic sugar
```

### 3. Block Arguments (Passing Functions to Methods)

**Before:**
```wren
list.reduce(0, Fn.new {|a, b| a + b})
list.each(Fn.new {|item| System.print(item) })
```

**After:**
```wren
list.reduce(0) {|a, b| a + b}
list.each {|item| System.print(item)}
```

## Commit f54f8df: "Remove Fn.new()"

This commit removed `Fn.new()` entirely, making `fn()` the only syntax for creating anonymous functions.

## Examples from Test Files

### Example 1: Function Arity
```wren
// Before:
System.print(Fn.new {}.arity)           // expect: 0
System.print(Fn.new {|a| a}.arity)     // expect: 1

// After:
System.print(fn () {}.arity)           // expect: 0
System.print(fn (a) {a}.arity)         // expect: 1
```

### Example 2: List Operations
```wren
// Before:
var max = Fn.new {|a, b| a > b ? a : b }
var sum = Fn.new {|a, b| a + b }
System.print(a.reduce(max))
System.print(a.reduce(10, max))

// After:
System.print(a.reduce {|a, b| a > b ? a : b })      // expect: 5
System.print(a.reduce(10) {|a, b| a > b ? a : b })  // expect: 10
```

### Example 3: Function Parameters
```wren
// Before:
var f0 = Fn.new { 0 }
var f1 = Fn.new {|a| a }
var f2 = Fn.new {|a, b| a + b }

// After:
fn f0() { 0 }
fn f1(a) { a }
fn f2(a, b) { a + b }
```

## Summary

1. **`Fn.new { ... }` → `fn () { ... }`**
   - The `fn` keyword replaces `Fn.new`
   - Parameters move from `|args|` syntax to `(args)` syntax
   - Block can be single expression or multiple statements in `{}`

2. **`fn.call(args)` → `fn(args)`**
   - The `.call()` is optional syntactic sugar
   - Functions can be called directly with `()`

3. **Block arguments:**
   - Functions passed as the last argument to a method can use trailing block syntax
   - Methods like `list.reduce(initial) {|a, b| ... }` instead of `list.reduce(initial, Fn.new {|a, b| ...})`
