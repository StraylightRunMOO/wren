// Ranges use negative indices: -1 (index 0), -2 (index 1), -3 (index 2), etc.
// This avoids 0 (which is now falsy) and preserves float precision

// Inclusive.
var range = 1..3
System.print(range.iterate(null)) // expect: -1
System.print(range.iterate(-1)) // expect: -2
System.print(range.iterate(-2)) // expect: -3
System.print(range.iterate(-3)) // expect: null
System.print(range.iterate(-4)) // expect: null

// Exclusive
range = 1...3
System.print(range.iterate(null)) // expect: -1
System.print(range.iterate(-1)) // expect: -2
System.print(range.iterate(-2)) // expect: null

// Descending inclusive range.
range = 3..1
System.print(range.iterate(null)) // expect: -1
System.print(range.iterate(-1)) // expect: -2
System.print(range.iterate(-2)) // expect: -3
System.print(range.iterate(-3)) // expect: null

// Descending exclusive range.
range = 3...1
System.print(range.iterate(null)) // expect: -1
System.print(range.iterate(-1)) // expect: -2
System.print(range.iterate(-2)) // expect: null

// Empty inclusive range.
range = 1..1
System.print(range.iterate(null)) // expect: -1
System.print(range.iterate(-1)) // expect: null

// Empty exclusive range.
range = 1...1
System.print(range.iterate(null)) // expect: null
