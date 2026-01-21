// False, null, 0, and empty collections are false.
System.print(false && "bad") // expect: false
System.print(null && "bad") // expect: null
System.print(0 && "bad") // expect: 0
System.print([] && "bad") // expect: []
System.print({} && "bad") // expect: {}

// Everything else is true.
System.print(true && "ok") // expect: ok
System.print(1 && "ok") // expect: ok
System.print("" && "ok") // expect: ok
System.print([1] && "ok") // expect: ok
System.print({1: 2} && "ok") // expect: ok
