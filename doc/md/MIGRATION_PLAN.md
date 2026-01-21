# Migration Plan: Update Wren Codebase to New Function Syntax

## Overview

Based on analysis of commits a45a436 and f54f8df from the functional branch, we need to migrate the codebase from old `Fn.new` syntax to new `fn` syntax.

## Syntax Transformations Required

### 1. Function Declaration

**From:** `var f = Fn.new {|args| body }`
**To:** `fn f(args) { body }`

**Examples:**
```wren
// Before:
var add = Fn.new {|a, b| a + b }
var noArgs = Fn.new { 42 }

// After:
fn add(a, b) { a + b }
fn noArgs() { 42 }
```

### 2. Function Calls

**From:** `f.call(args)`
**To:** `f(args)`

**Examples:**
```wren
// Before:
var result = add.call(2, 3)
noArgs.call()

// After:
var result = add(2, 3)
noArgs()
```

### 3. Block Arguments (Functions as Method Arguments)

**From:** `method(arg, Fn.new {|x| ... })`
**To:** `method(arg) {|x| ... }`

**Examples:**
```wren
// Before:
var max = Fn.new {|a, b| a > b ? a : b }
list.reduce(0, max)

// After:
list.reduce(0) {|a, b| a > b ? a : b }

// Or with variables:
var sum = Fn.new {|a, b| a + b }
list.reduce(sum)  // No change if not passing as block argument
list.reduce(0, sum)  // Before
list.reduce(0) {|a, b| a + b }  // After
```

## Files to Update

### Test Files (High Priority)

#### Core Function Tests
- `test/core/function/arity.wren` - Function arity tests
- `test/core/function/equality.wren` - Function equality tests
- `test/core/function/to_string.wren` - toString tests
- `test/core/function/type.wren` - Type tests
- `test/core/function/call_missing_arguments.wren` - Error handling
- `test/core/function/call_extra_arguments.wren` - Error handling
- `test/core/function/call_runtime_error.wren` - Error handling
- `test/core/function/new_wrong_arg_type.wren` - Can be deleted (tests Fn.new validation)

#### List Operation Tests
- `test/core/list/reduce.wren` - Demonstrates block argument syntax
- `test/core/list/map.wren`
- `test/core/list/each.wren`
- `test/core/list/where.wren`
- `test/core/list/count_predicate.wren`

#### Language Feature Tests
- `test/language/function/parameters.wren` - All use .call() syntax
- `test/language/function/syntax.wren`
- `test/language/function/empty_body.wren`
- `test/language/function/newline_body.wren`
- `test/language/function/no_newline_before_close.wren`
- `test/language/function/newline_in_expression_block.wren`

#### Closure Tests (Multiple Files)
- All files in `test/language/closure/` - Use Fn.new extensively
- All files in `test/language/field/` - Use closures
- All files in `test/language/static_field/` - Use closures
- All files in `test/language/this/` - Use closures
- All files in `test/language/super/` - Use closures
- All files in `test/language/for/` - Use closures
- All files in `test/language/while/` - Use closures
- All files in `test/language/break/` - Use closures
- All files in `test/language/continue/` - Use closures
- All files in `test/language/return/` - Use closures
- All files in `test/language/assignment/` - May use closures
- All files in `test/language/nonlocal/` - Use closures
- All files in `test/language/fiber/` - Use closures
- All files in `test/language/inheritance/` - Inherit from closures

#### Limit Tests
- `test/limit/long_function.wren`
- `test/limit/too_many_function_parameters.wren`
- `test/limit/too_many_constants.wren`
- `test/limit/many_constants.wren`
- `test/limit/reuse_constants.wren`

#### Other Tests
- `test/core/object/is.wren` - Tests type checking with functions
- `test/benchmark/delta_blue.wren` - Benchmark uses functions
- `test/benchmark/api_call.wren` - API call tests
- `test/benchmark/fibers.wren` - Fiber tests

### Documentation Files

- `doc/site/functions.markdown` - Core language documentation
- `doc/site/modules/core/fn.markdown` - Fn module documentation
- `doc/site/modules/meta/meta.markdown` - Meta module docs (if uses functions)

### Examples

- `example/skynet.wren` - Skynet benchmark example

### Demo Files (I Created These - Already Updated)

- `demo_fn_call_sugar.wren` ✓ Already uses new syntax
- `test_fn_call_sugar.wren` ✓ Already uses new syntax
- `test_comprehensive.wren` ✓ Already uses new syntax

## Implementation Strategy

### Phase 1: Core Function Tests
Update tests in `test/core/function/` to use new syntax. These are fundamental and other tests depend on them.

### Phase 2: List Operation Tests
Update list operations that demonstrate block argument syntax.

### Phase 3: Language Feature Tests
Update function syntax tests, then closure tests.

### Phase 4: All Other Tests
Systematically update remaining test files.

### Phase 5: Documentation
Update documentation to reflect new syntax.

### Phase 6: Examples
Update example files.

## Special Considerations

1. **Block Argument Syntax**: When a function is passed as the last argument to a method, it can use the trailing block syntax: `method(arg) {|x| ...}` instead of `method(arg, fn (x) { ... })`

2. **Call Syntax**: The `.call()` method still works but is redundant. Can be replaced with direct call: `f()` instead of `f.call()`

3. **Function Naming**: When assigning functions to variables, the syntax changes from `var f = Fn.new {...}` to `fn f() {...}`

4. **Arity Syntax**: Parameter syntax changes from `|a, b, c|` to `(a, b, c)`

5. **File to Delete**: `test/core/function/new_wrong_arg_type.wren` tests `Fn.new()` validation which no longer exists.

## Verification

After each phase, run tests to ensure:
1. Syntax is correct
2. Function behavior is preserved
3. Error messages still work correctly
4. No regressions introduced
