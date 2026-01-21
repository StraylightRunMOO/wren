// "abcd" has 4 bytes, so with 1-based iterators, valid values are 1-4
"abcd".bytes.iteratorValue(5) // expect runtime error: Index out of bounds.
