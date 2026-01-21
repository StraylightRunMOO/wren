var s = "abçd"
System.print(s.iterate(null)) // expect: 1
System.print(s.iterate(1)) // expect: 2
System.print(s.iterate(2)) // expect: 3
// Skip 4 because that's the middle of the ç sequence.
System.print(s.iterate(3)) // expect: 5
// Iterating from the middle of a UTF-8 sequence goes to the next one.
System.print(s.iterate(4)) // expect: 5
System.print(s.iterate(5)) // expect: null

// Out of bounds.
System.print(s.iterate(123)) // expect: null
System.print(s.iterate(-1)) // expect: null

// Nothing to iterate in an empty string.
System.print("".iterate(null)) // expect: null

// 8-bit clean.
System.print("a\0b\0c".iterate(null)) // expect: 1
System.print("a\0b\0c".iterate(1)) // expect: 2
System.print("a\0b\0c".iterate(2)) // expect: 3
System.print("a\0b\0c".iterate(3)) // expect: 4
System.print("a\0b\0c".iterate(4)) // expect: 5
System.print("a\0b\0c".iterate(5)) // expect: null

// Iterates over invalid UTF-8 one byte at a time.
System.print("\xef\xf7".iterate(null)) // expect: 1
System.print("\xef\xf7".iterate(1)) // expect: 2
System.print("\xef\xf7".iterate(2)) // expect: null
