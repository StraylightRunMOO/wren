// Test precedence
var f = fn { "woo" }
var result = f()
System.print(result)

// Test inline
System.print((fn { "woo" })())

// Test method call
System.print(fn {}.arity)
