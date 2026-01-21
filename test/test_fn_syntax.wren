var add = fn (a, b) { a + b }
System.print("add(5, 3) = %(add(5, 3))")

var noArgs = fn () { "no args" }
System.print("noArgs() = %(noArgs())")

var multiply = fn (x, y) { x * y }
var result = multiply(6, 7)
System.print("Result: %(result)")

var list = [1, 2, 3, 4, 5]
var doubled = list.map {|n| n * 2}
System.print("doubled = %(doubled)")

System.print("\nAll tests passed!")
