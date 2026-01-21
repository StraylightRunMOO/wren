// Bytes:           11111
//        012345678901234
// Chars: sø mé ஃ  thî ng
var codePoints = "søméஃthîng".codePoints

// Iterators are now 1-based (1, 2, 3... instead of 0, 1, 2...)
System.print(codePoints.iteratorValue(1)) // expect: 115
System.print(codePoints.iteratorValue(2)) // expect: 248
System.print(codePoints.iteratorValue(3)) // expect: -1
System.print(codePoints.iteratorValue(4)) // expect: 109
System.print(codePoints.iteratorValue(5)) // expect: 233
System.print(codePoints.iteratorValue(6)) // expect: -1
System.print(codePoints.iteratorValue(7)) // expect: 2947
System.print(codePoints.iteratorValue(8)) // expect: -1
System.print(codePoints.iteratorValue(9)) // expect: -1
System.print(codePoints.iteratorValue(10)) // expect: 116
System.print(codePoints.iteratorValue(11)) // expect: 104
System.print(codePoints.iteratorValue(12)) // expect: 238
System.print(codePoints.iteratorValue(13)) // expect: -1
System.print(codePoints.iteratorValue(14)) // expect: 110
System.print(codePoints.iteratorValue(15)) // expect: 103

System.print("\0".codePoints.iteratorValue(1)) // expect: 0

// Returns -1 for invalid UTF-8 sequences.
System.print("\xef\xf7".codePoints.iteratorValue(1)) // expect: -1
System.print("\xef\xf7".codePoints.iteratorValue(2)) // expect: -1
