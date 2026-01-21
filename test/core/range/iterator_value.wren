var range = 1..3
// Ranges use negative indices: -1 (index 0) -> 1, -2 (index 1) -> 2, -3 (index 2) -> 3
System.print(range.iteratorValue(-1)) // expect: 1
System.print(range.iteratorValue(-2)) // expect: 2
System.print(range.iteratorValue(-3)) // expect: 3

// Can use any negative number - it's decoded as an index
System.print(range.iteratorValue(-4)) // expect: 4
System.print(range.iteratorValue(-5)) // expect: 5
