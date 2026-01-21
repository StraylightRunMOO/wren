# Summary of Changes: fn.call Syntactic Sugar

This document summarizes the changes made to add syntactic sugar for `fn.call()` in Wren.

## Changes Made

### 1. Compiler Changes (`src/vm/wren_compiler.c`)

Added a new `call()` function to handle infix parenthesized calls as syntactic sugar for invoking the "call" method:

- Added `static void call(Compiler* compiler, bool allowAssignment)` function that:
  - Creates a signature for the "call" method
  - Parses the argument list (allowing empty arguments)
  - Calls `callSignature()` to emit the appropriate bytecode

- Renamed the existing `call()` function to `dot()` to handle method calls after the dot operator

- Updated the grammar rules table:
  - Changed `TOKEN_LEFT_PAREN` from `PREFIX(grouping)` to `{ grouping, call, NULL, PREC_CALL, NULL }`
  - Changed `TOKEN_DOT` from `INFIX(PREC_CALL, call)` to `INFIX(PREC_CALL, dot)`

### 2. Test Updates

Updated test files to use the new syntactic sugar:

- `test/core/function/call_extra_arguments.wren` - Converted from `fn.call(args)` to `fn(args)`
- `test/core/function/call_missing_arguments.wren` - Converted from `fn.call(args)` to `fn(args)`

### 3. Documentation Updates

- `doc/site/modules/core/fn.markdown` - Added documentation explaining that `fn(args)` is syntactic sugar for `fn.call(args)`

## What This Enables

Before these changes, calling a function required the verbose syntax:

```dart
var fn = Fn.new { |a, b| a + b }
fn.call(5, 3)  // Returns 8
```

After these changes, the same call can be written more concisely:

```dart
var fn = Fn.new { |a, b| a + b }
fn(5, 3)  // Returns 8, same as fn.call(5, 3)
```

Both forms are equivalent and work identically:
- They accept the same arguments
- They handle extra arguments the same way (extra args are ignored)
- They handle missing arguments the same way (runtime error)
- They work in all contexts (variable assignments, list elements, etc.)

## Testing

All existing tests pass with the new syntax:
- Function arity tests
- Function call tests with extra/missing arguments
- Precedence tests
- Closure tests

The changes are backward compatible - the old `fn.call(args)` syntax continues to work exactly as before.

## Implementation Details

The syntactic sugar is implemented at the compiler level by treating an infix `(` token (when it follows an expression) as a special case that generates a call to the "call" method. This is similar to how other languages like Ruby handle function/method calls.

The implementation:
1. Detects when a `(` follows an expression (not just after a prefix `(` for grouping)
2. Creates a method signature for "call" with the parsed arguments
3. Emits the appropriate CALL bytecode

This approach is efficient and integrates cleanly with Wren's existing method call machinery.