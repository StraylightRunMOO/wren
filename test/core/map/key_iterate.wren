var a = {"one": 1, "two": 2, "three": 3, "four": 4}.keys

// The precise numeric values aren't defined since they are indexes into the
// entry table and the hashing process isn't specified. So we just validate
// what we can assume about them. Iterators are now 1-based.

System.print(a.iterate(null) is Num) // expect: true
System.print(a.iterate(null) >= 1) // expect: true

System.print(a.iterate(1) is Num) // expect: true
System.print(a.iterate(1) > 1) // expect: true
System.print(a.iterate(2) is Num) // expect: true
System.print(a.iterate(2) > 1) // expect: true
System.print(a.iterate(3) is Num) // expect: true
System.print(a.iterate(3) > 1) // expect: true
System.print(a.iterate(4) is Num) // expect: true
System.print(a.iterate(4) > 1) // expect: true

var previous = 0
var iterator = a.iterate(null)
while (iterator) {
  System.print(iterator > previous)
  System.print(iterator is Num)
  previous = iterator
  iterator = a.iterate(iterator)
}
// First entry:
// expect: true
// expect: true
// Second entry:
// expect: true
// expect: true
// Third entry:
// expect: true
// expect: true
// Fourth entry:
// expect: true
// expect: true

// Out of bounds.
System.print(a.iterate(16)) // expect: null
System.print(a.iterate(-1)) // expect: null

// Nothing to iterate in an empty map.
System.print({}.keys.iterate(null)) // expect: null
