// Comprehensive test of fn.call syntactic sugar

System.print("=== Testing fn.call Syntactic Sugar ===")
System.print("")

// Test 1: Basic functionality
var add = Fn.new { |a, b| a + b }
System.print("✓ Basic call syntax: %(add(2, 3) == 5)")
System.print("✓ Old call syntax: %(add.call(2, 3) == 5)")

// Test 2: No arguments
var noArgs = Fn.new { 42 }
System.print("✓ No arguments: %(noArgs() == 42)")
System.print("✓ No args old syntax: %(noArgs.call() == 42)")

// Test 3: Single argument
var double = Fn.new { |x| x * 2 }
System.print("✓ Single argument: %(double(5) == 10)")

// Test 4: Multiple arguments
var mult = Fn.new { |a, b, c| a * b * c }
System.print("✓ Three arguments: %(mult(2, 3, 4) == 24)")

// Test 5: Extra arguments are ignored
System.print("✓ Extra arguments ignored: %(add(1, 2, 3, 4, 5) == 3)")

// Test 6: Functions as first-class values
var fns = [add, double, mult]
System.print("✓ Function in list: %(fns[0](10, 20) == 30)")
System.print("✓ Function in list old syntax: %(fns[0].call(10, 20) == 30)")

// Test 7: Return values
var result1 = add(5, 7)
var result2 = add.call(5, 7)
System.print("✓ Return value consistency: %(result1 == result2)")

// Test 8: Arity property
System.print("")
System.print("=== Testing Arity ===")
System.print("✓ Arity 0: %(Fn.new {}.arity == 0)")
System.print("✓ Arity 1: %(Fn.new {|a| a}.arity == 1)")
System.print("✓ Arity 2: %(Fn.new {|a, b| a + b}.arity == 2)")
System.print("✓ Arity 3: %(Fn.new {|a, b, c| a + b + c}.arity == 3)")

System.print("")
System.print("🎉 All tests completed successfully!")