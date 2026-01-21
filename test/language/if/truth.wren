// False, null, 0, and empty collections are false.
if (false) System.print("bad") else System.print("false") // expect: false
if (null) System.print("bad") else System.print("null") // expect: null
if (0) System.print("bad") else System.print("zero") // expect: zero
if (0.0) System.print("bad") else System.print("zero-float") // expect: zero-float
if ([]) System.print("bad") else System.print("empty-list") // expect: empty-list
if ({}) System.print("bad") else System.print("empty-map") // expect: empty-map

// Everything else is true.
if (true) System.print(true) // expect: true
if (1) System.print(1) // expect: 1
if ("") System.print("empty") // expect: empty
if ([1]) System.print("list") // expect: list
if ({1: 2}) System.print("map") // expect: map
