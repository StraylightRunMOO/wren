// Test both fn syntaxes
var noParens = fn { 42 }
System.print("No parens: %(noParens())")

var withParens = fn () { 42 }
System.print("With parens: %(withParens())")

var withParams = fn (x, y) { x + y }
System.print("With params: %(withParams(5, 3))")

var list = [1, 2, 3, 4, 5]
var mapped = list.map {|n| n * 2}
System.print("Mapped: %(mapped)")

System.print("\n✅ All fn syntaxes work!")
