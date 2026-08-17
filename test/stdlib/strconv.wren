import "strconv" for strconv

System.print(strconv.parseInt("10") == 10)  // expect: true
System.print(strconv.formatInt(255, 16) == "ff")  // expect: true
