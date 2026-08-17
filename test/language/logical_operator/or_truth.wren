// False, null, and 0 are false.
System.print(false || "ok") // expect: ok
System.print(null || "ok") // expect: ok
System.print(0 || "ok") // expect: ok

// Everything else is true, including empty collections.
System.print(true || "ok") // expect: true
System.print(1 || "ok") // expect: 1
System.print("s" || "ok") // expect: s
System.print([] || "ok") // expect: []
System.print({} || "ok") // expect: {}
System.print([1] || "ok") // expect: [1]
System.print({1: 2} || "ok") // expect: {1: 2}
