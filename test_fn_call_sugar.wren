// Test fn.call syntactic sugar

var fn = Fn.new { |a, b|
  System.print("Result: %(a + b)")
}

// Test the old syntax still works
fn.call(10, 20)

// Test the new syntactic sugar
fn(30, 40)

// Test with no arguments
var fn2 = Fn.new {
  System.print("No args")
}
fn2()

// Test with one argument
var fn3 = Fn.new { |x| System.print("One arg: %(x)") }
fn3(99)