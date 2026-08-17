import "os" for os, Stdin, Stdout, Stderr

System.print(os.stdin == 0)   // expect: true
System.print(os.stdout == 1)  // expect: true
System.print(os.stderr == 2)  // expect: true
System.print(Stdin == 0)      // expect: true
System.print(Stdout == 1)     // expect: true
System.print(Stderr == 2)     // expect: true
System.print(os.args is List) // expect: true
