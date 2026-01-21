// False, null, 0, and empty collections are false.
System.print(false || "ok") // expect: ok
System.print(null || "ok") // expect: ok
System.print(0 || "ok") // expect: ok
System.print([] || "ok") // expect: ok
System.print({} || "ok") // expect: ok

// Everything else is true.
System.print(true || "ok") // expect: true
System.print(1 || "ok") // expect: 1
System.print("s" || "ok") // expect: s
System.print([1] || "ok") // expect: [1]
System.print({1: 2} || "ok") // expect: {1: 2}
