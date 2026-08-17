System.print("a" < "b")          // expect: true
System.print("b" < "a")          // expect: false
System.print("abc" < "abd")      // expect: true
System.print("abc" < "abc")      // expect: false
System.print("ab" < "abc")       // expect: true

System.print("b" > "a")          // expect: true
System.print("a" > "b")          // expect: false
System.print("abc" > "ab")       // expect: true

System.print("abc" <= "abc")     // expect: true
System.print("abc" <= "abd")     // expect: true
System.print("abd" <= "abc")     // expect: false

System.print("abc" >= "abc")     // expect: true
System.print("abd" >= "abc")     // expect: true
System.print("abc" >= "abd")     // expect: false

// 8-bit clean: embedded NUL must not truncate the compare.
System.print("a\0c" < "a\0d")    // expect: true
System.print("a\0c" > "a")       // expect: true
System.print("a" < "a\0c")       // expect: true
System.print("a\0c" == "a\0c")   // expect: true
System.print("a\0c" < "a\0c")    // expect: false
