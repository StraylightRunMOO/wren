// Method calls without parentheses
class Foo {
  construct new() {}

  method {
    return "called"
  }

  methodWithArg(x) {
    return "arg: %(x)"
  }

  getValue { 42 }

  static staticMethod {
    return "static called"
  }
}

var foo = Foo.new()

// Instance method without parens
System.print(foo.method) // expect: called

// Getter (already worked)
System.print(foo.getValue) // expect: 42

// Static method without parens
System.print(Foo.staticMethod) // expect: static called

// Chain with field access
class Bar {
  construct new() { _foo = Foo.new() }

  foo { _foo }
}

var bar = Bar.new()
System.print(bar.foo.method) // expect: called
