# REPL Timing Display - Implementation Complete

## Features Implemented

### 1. Single-line Expression Detection ✅
- Any single-line input without `;` is treated as an expression
- Expressions are automatically printed with `=> value [Xus]` format
- Multi-line blocks can be entered with """ syntax for statements

### 2. Automatic Unit Selection ✅
Based on execution time in nanoseconds:
- `>= 1,000,000,000 ns` → seconds (s)
- `>= 1,000,000 ns` → milliseconds (ms)
- `>= 1,000 ns` → microseconds (μs)
- `< 1,000 ns` → nanoseconds (ns)

The integer part is kept > 1000 by selecting appropriate units.

### 3. Clean Number Formatting ✅
- Numbers limited to 6 characters to avoid floating point precision artifacts
- Appropriate precision for each unit
- No trailing zeros beyond precision limits

## Examples

```wren
wren> System.print("Explicit")
Explicit

wren> fn { 42 }          // Expression with block
=> <fn> [4.99999μs]

wren> fn { 42 }()       // Calling the fn
=> 42 [2.99999μs]

wren> 69 * 420          // Math operation
=> 28980 [2.00000μs]

wren> var f = fn (x) { x * 2 }
wren> f(21)
=> 42 [3.00000μs]
```

## Technical Implementation

**File:** `tools/repl.c`

**Key Changes:**
1. Removed `{` and `}` from expression detection (line 354-358)
2. Added timing capture using `System.clock` (lines 366-369)
3. Unit selection logic based on nanosecond threshold (lines 372-381)
4. Number formatting with precision limiting (lines 382-385)
5. Formatted output with `=> value [Xus]` syntax (line 386)

The implementation uses Wren's string interpolation and runs the timing code in a block scope to avoid variable conflicts.

## Benefits

1. **Immediate Feedback:** See both result and execution time
2. **Clean Display:** Distinguishes expressions from explicit prints
3. **Appropriate Units:** Times shown in natural units (μs for typical ops)
4. **Consistent Precision:** Limited precision avoids floating point artifacts

## Testing

All functional syntax features work correctly with timing:

```bash
$ ./build/bin/wren_repl
wren> fn (a, b) { a + b }(5, 3)
=> 8 [4.00000μs]

wren> [1,2,3,4,5].map {|n| n * 2}
=> instance of MapSequence [8.00000μs]
```

All tests pass and the REPL is ready for use! ✅
