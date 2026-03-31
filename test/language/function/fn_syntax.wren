// fn syntax tests

// Basic fn without parens
var f1 = fn { 42 }
System.print(f1()) // expect: 42

// fn with empty parens
var f2 = fn () { 43 }
System.print(f2()) // expect: 43

// fn with parameters
var f3 = fn (a) { a * 2 }
System.print(f3(5)) // expect: 10

var f4 = fn (a, b) { a + b }
System.print(f4(3, 4)) // expect: 7

// Direct invocation
System.print(fn { 100 }()) // expect: 100
System.print(fn (x) { x * 2 }(5)) // expect: 10

// fn arity property
System.print(fn {}.arity) // expect: 0
System.print(fn (a, b) {}.arity) // expect: 2

// fn type
System.print(fn {} is Fn) // expect: true

// Nested fn - need explicit return when body has multiple statements
var outer = fn {
  var inner = fn { "inner" }
  return inner()
}
System.print(outer()) // expect: inner

// Closure capturing - returns a fn that can be called
var makeAdder = fn (x) {
  return fn (y) { x + y }
}
var add5 = makeAdder(5)
System.print(add5(3)) // expect: 8
