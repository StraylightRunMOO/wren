// Test what parses
fn { 42 }
fn () { 42 }
var f = fn { 42 }
System.print(f())
System.print((fn { 42 })())
