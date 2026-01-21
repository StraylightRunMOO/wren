// Bytes:
//        012345678
// Chars: sø mé ஃ
var bytes = "søméஃ".bytes

// Iterators are now 1-based (1, 2, 3... instead of 0, 1, 2...)
System.print(bytes.iteratorValue(1)) // expect: 115
System.print(bytes.iteratorValue(2)) // expect: 195
System.print(bytes.iteratorValue(3)) // expect: 184
System.print(bytes.iteratorValue(4)) // expect: 109
System.print(bytes.iteratorValue(5)) // expect: 195
System.print(bytes.iteratorValue(6)) // expect: 169
System.print(bytes.iteratorValue(7)) // expect: 224
System.print(bytes.iteratorValue(8)) // expect: 174
System.print(bytes.iteratorValue(9)) // expect: 131
