// Test new fn syntax
System.print("Testing new fn syntax:")

var add = fn (a, b) { a + b }
System.print("add(5, 3) = %(add(5, 3))")

var noArgs = fn () { "no args" }
System.print("noArgs() = %(noArgs())")

var oneArg = fn (x) { "one: %(x)" }
System.print("oneArg(42) = %(oneArg(42))")

var twoArgs = fn (x, y) { "two: %(x), %(y)" }
System.print("twoArgs(\"hello\", \"world\") = %(twoArgs("hello", "world"))")

// Test block arguments
var list = [1, 2, 3, 4, 5]
var doubled = list.map {|n| n * 2}
System.print("doubled = %(doubled)")

var filtered = list.where {|n| n > 3}
System.print("filtered = %(filtered)")

System.print("\\nAll tests passed!")
