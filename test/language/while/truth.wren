// False, null, 0, and empty collections are false.
while (false) {
  System.print("bad")
  break
}

while (null) {
  System.print("bad")
  break
}

while (0) {
  System.print("bad")
  break
}

while ([]) {
  System.print("bad")
  break
}

while ({}) {
  System.print("bad")
  break
}

// Everything else is true.
while (true) {
  System.print("true") // expect: true
  break
}

while (1) {
  System.print(1) // expect: 1
  break
}

while ("") {
  System.print("string") // expect: string
  break
}

while ([1]) {
  System.print("list") // expect: list
  break
}
