var s = "abçd"
System.print(s.iteratorValue(1)) // expect: a
System.print(s.iteratorValue(2)) // expect: b
System.print(s.iteratorValue(3)) // expect: ç
// Iterator value in middle of UTF sequence is the unencoded byte.
System.print(s.iteratorValue(4) == "\xa7") // expect: true
System.print(s.iteratorValue(5)) // expect: d

// 8-bit clean.
var t = "a\0b\0c"
System.print(t.iteratorValue(1) == "a") // expect: true
System.print(t.iteratorValue(2) == "\0") // expect: true
System.print(t.iteratorValue(3) == "b") // expect: true
System.print(t.iteratorValue(4) == "\0") // expect: true
System.print(t.iteratorValue(5) == "c") // expect: true

// Returns single byte strings for invalid UTF-8 sequences.
System.print("\xef\xf7".iteratorValue(1) == "\xef") // expect: true
System.print("\xef\xf7".iteratorValue(2) == "\xf7") // expect: true
