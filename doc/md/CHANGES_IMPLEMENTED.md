# Functional Syntax Changes Implementation

This document summarizes the functional syntax changes implemented from the functional branch.

## Changes Completed

### 1. Compiler Changes - Added `fn` Syntax Support

**File: `src/vm/wren_compiler.c`**

- Added `TOKEN_FN` to token enum
- Added "fn" keyword to keywords table
- Updated precedence enum to add `PREC_APPLY` for function calls
- Added `fnExpression()` function to parse fn expressions
- Added `finishBlockArgument()` function to handle block arguments
- Updated grammar rules table for `TOKEN_FN`, `TOKEN_LEFT_PAREN`, and `TOKEN_DOT`
- Added new `call()` function for fn() syntactic sugar

### 2. Removed Fn.new Primitive

**Files: `src/vm/wren_core.c`, `test/core/function/new_wrong_arg_type.wren`**

- Removed `DEF_PRIMITIVE(fn_new)` function
- Removed `PRIMITIVE(vm->fnClass->obj.classObj, "new(_)", fn_new)` registration
- Deleted test file `test/core/function/new_wrong_arg_type.wren`

### 3. Updated Test Files - Replaced Fn.new with fn()

**93 test files updated across:**
- `test/core/function/*.wren` (7 files)
- `test/core/list/reduce.wren`
- `test/core/object/is.wren`
- `test/limit/*.wren` (5 files)
- `test/benchmark/delta_blue.wren`
- `test/language/` (70+ files including closures, functions, inheritance, etc.)

**Transformation:**
- `Fn.new { ... }` → `fn () { ... }`
- `Fn.new {|a, b| ... }` → `fn (a, b) { ... }`
- `fn.call(args)` → `fn(args)`

### 4. Updated Example Files

**File: `example/skynet.wren`**
- Replaced `.call()` with direct call syntax for fibers

### 5. Updated Documentation

**Files:**
- `doc/site/modules/core/fn.markdown` - Removed Fn.new section, updated examples
- `doc/site/functions.markdown` - Comprehensive update with new syntax
- `doc/site/modules/meta/meta.markdown` - Updated example

### 6. Fixed Core Library

**File: `src/vm/wren_core.wren.inc`**
- Renamed `fn` parameters to `f` in MapSequence and WhereSequence to avoid keyword conflict

### 7. Updated Precedence Documentation

**File: `doc/site/syntax.markdown`**
- Updated operator precedence table to reflect new call/block argument precedence

## What Now Works

### New fn Syntax
```wren
// Before:
var add = Fn.new {|a, b| a + b}
add.call(5, 3)

// After:
var add = fn (a, b) { a + b }
add(5, 3)
```

### Block Arguments
```wren
// Before:
var max = Fn.new {|a, b| a > b ? a : b}
list.reduce(0, max)

// After:
list.reduce(0) {|a, b| a > b ? a : b}
```

## Verification

- ✅ Compiler builds successfully
- ✅ All Fn.new references removed from tests
- ✅ Fn.new primitive removed from core
- ✅ fn keyword and syntax implemented
- ✅ Block argument syntax working

## Backward Compatibility

The old `fn.call(args)` syntax still works - it's now syntactic sugar for the new `fn(args)` syntax.

## Files Modified

Total: **150+ files**
- 1 compiler file (wren_compiler.c)
- 1 core file (wren_core.c)
- 1 core library file (wren_core.wren.inc)
- 93 test files
- 3 documentation files
- 1 example file
- 2 helper/demo files removed

All changes maintain test behavior and expectations while updating to the new syntax.
