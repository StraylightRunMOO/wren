var f = fn {}
System.print(f.arity) // expect: 0

var g = fn (a, b) { a }
System.print(g.arity) // expect: 2
