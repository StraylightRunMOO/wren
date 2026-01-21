// "string" has 6 bytes, so with 1-based iterators, valid values are 1-6
System.print("string".codePoints.iteratorValue(7)) // expect runtime error: Index out of bounds.
