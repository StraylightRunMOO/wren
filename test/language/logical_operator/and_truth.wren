// False, null, and 0 are false.
System.print(false && "bad") // expect: false
System.print(null && "bad") // expect: null
System.print(0 && "bad") // expect: 0

// Everything else is true, including empty collections.
System.print(true && "ok") // expect: ok
System.print(1 && "ok") // expect: ok
System.print("" && "ok") // expect: ok
System.print([] && "ok") // expect: ok
System.print({} && "ok") // expect: ok
System.print([1] && "ok") // expect: ok
System.print({1: 2} && "ok") // expect: ok
