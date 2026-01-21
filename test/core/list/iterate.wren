var a = ["one", "two", "three", "four"]
System.print(a.iterate(null)) // expect: 1
System.print(a.iterate(1)) // expect: 2
System.print(a.iterate(2)) // expect: 3
System.print(a.iterate(3)) // expect: 4
System.print(a.iterate(4)) // expect: null

// Out of bounds.
System.print(a.iterate(123)) // expect: null
System.print(a.iterate(-1)) // expect: null

// Nothing to iterate in an empty list.
System.print([].iterate(null)) // expect: null
