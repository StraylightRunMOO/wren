// Not structurally equal.
System.print(fn { 123 } == fn { 123 })  // expect: false
System.print(fn { 123 } != fn { 123 })  // expect: true

// Not equal to other types.
System.print(fn { 123 } == 1)         // expect: false
System.print(fn { 123 } == false)     // expect: false
System.print(fn { 123 } == "fn 123")  // expect: false
System.print(fn { 123 } != 1)         // expect: true
System.print(fn { 123 } != false)     // expect: true
System.print(fn { 123 } != "fn 123")  // expect: true

// Equal by identity.
var f = fn { 123 }
System.print(f == f) // expect: true
System.print(f != f) // expect: false

// Closures for the same function are not equal.
var fns = []
for (i in 1..2) {
  fns.add(fn { 123 })
}
System.print(fns[0] == fns[1]) // expect: false
