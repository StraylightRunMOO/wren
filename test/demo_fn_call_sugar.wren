// Demonstration of fn.call syntactic sugar

System.print("=== fn.call Syntactic Sugar Demo ===")
System.print("")

// Old syntax still works
System.print("1. Old syntax (fn.call(args)):")
var add = Fn.new { |a, b| a + b }
System.print("   add.call(5, 3) = %(add.call(5, 3))")

System.print("")
System.print("2. New syntactic sugar (fn(args)):")
System.print("   add(5, 3) = %(add(5, 3))")

System.print("")
System.print("3. Works with any number of arguments:")
var noArgs = Fn.new { "no args" }
var oneArg = Fn.new { |x| "one: %(x)" }
var twoArgs = Fn.new { |x, y| "two: %(x), %(y)" }

System.print("   No args: %(noArgs())")
System.print("   One arg: %(oneArg(42))")
System.print("   Two args: %(twoArgs("hello", "world"))")

System.print("")
System.print("4. Extra arguments are ignored:")
var takesTwo = Fn.new { |a, b| "got %(a) and %(b)" }
System.print("   %(takesTwo(1, 2, 3, 4, 5))")

System.print("")
System.print("5. Works in all contexts:")
var multiply = Fn.new { |x, y| x * y }
var result = multiply(6, 7)
System.print("   Result: %(result)")

var fnList = [add, multiply]
System.print("   From list: %(fnList[0](10, 20))")

System.print("")
System.print("=== Demo Complete ===")