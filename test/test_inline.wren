// Test inline fn with method call
var result = (fn {}).arity
System.print(result) // expect: 0

var result2 = fn (a, b) { a }.arity
System.print(result2) // expect: 2
