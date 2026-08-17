class Iter {
  construct new(value) { _value = value }
  iterate(iterator) { _value }
  iteratorValue(iterator) { "value" }
}

// False, null, and 0 stop iteration. Empty collections do not.
for (n in Iter.new(false)) {
  System.print("bad")
  break
}

for (n in Iter.new(null)) {
  System.print("bad")
  break
}

for (n in Iter.new(0)) {
  System.print("bad")
  break
}

for (n in Iter.new([])) {
  System.print("empty-list") // expect: empty-list
  break
}

for (n in Iter.new({})) {
  System.print("empty-map") // expect: empty-map
  break
}

// Everything else is true.
for (n in Iter.new(true)) {
  System.print("true") // expect: true
  break
}

for (n in Iter.new(1)) {
  System.print(1) // expect: 1
  break
}

for (n in Iter.new("")) {
  System.print("string") // expect: string
  break
}

for (n in Iter.new([1])) {
  System.print("list") // expect: list
  break
}
