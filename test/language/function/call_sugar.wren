// .call() syntactic sugar - fn(args) is equivalent to fn.call(args)

var f = fn { 42 }

// Both syntaxes should work
System.print(f.call()) // expect: 42
System.print(f()) // expect: 42

// With parameters
var add = fn (a, b) { a + b }
System.print(add.call(3, 4)) // expect: 7
System.print(add(3, 4)) // expect: 7

// Direct invocation sugar
System.print(fn { "hello" }()) // expect: hello
System.print(fn (x) { x * 2 }(5)) // expect: 10

// Method access still works
System.print(f.arity) // expect: 0

// Counter using a class
class Counter {
  construct new(start) {
    _count = start
  }

  next {
    _count = _count + 1
    return _count
  }
}

var counter = Counter.new(0)
System.print(counter.next) // expect: 1
System.print(counter.next) // expect: 2
System.print(counter.next) // expect: 3
