// strconv comprehensive tests
import "strconv" for strconv, Atoi, Atof, Itoa, Ftoa

// Test parseInt with default base 10
System.print(strconv.parseInt("42") == 42)  // expect: true
System.print(strconv.parseInt("-123") == -123)  // expect: true
System.print(strconv.parseInt("0") == 0)  // expect: true

// Test parseInt with different bases
System.print(strconv.parseInt("ff", 16) == 255)  // expect: true
System.print(strconv.parseInt("FF", 16) == 255)  // expect: true
System.print(strconv.parseInt("100", 2) == 4)  // expect: true
System.print(strconv.parseInt("77", 8) == 63)  // expect: true
System.print(strconv.parseInt("z", 36) == 35)  // expect: true

// Test parseFloat
System.print(strconv.parseFloat("3.14") == 3.14)  // expect: true
System.print(strconv.parseFloat("-2.5") == -2.5)  // expect: true
System.print(strconv.parseFloat("0.0") == 0.0)  // expect: true

// Test formatInt
System.print(strconv.formatInt(42) == "42")  // expect: true
System.print(strconv.formatInt(-123) == "-123")  // expect: true
System.print(strconv.formatInt(255, 16) == "ff")  // expect: true
System.print(strconv.formatInt(4, 2) == "100")  // expect: true
System.print(strconv.formatInt(63, 8) == "77")  // expect: true

// Test formatFloat
System.print(strconv.formatFloat(3.14).startsWith("3.14"))  // expect: true
System.print(strconv.formatFloat(-2.5) == "-2.5")  // expect: true

// Test quote/unquote
var quoted = strconv.quote("hello")
System.print(quoted is String)  // expect: true
var unquoted = strconv.unquote(quoted)
System.print(unquoted == "hello")  // expect: true

// Test isInt
System.print(strconv.isInt("123") == true)  // expect: true
System.print(strconv.isInt("-456") == true)  // expect: true
System.print(strconv.isInt("12.3") == false)  // expect: true
System.print(strconv.isInt("abc") == false)  // expect: true
System.print(strconv.isInt(123) == false)  // expect: true

// Test isFloat
System.print(strconv.isFloat("3.14") == true)  // expect: true
System.print(strconv.isFloat("-2.5") == true)  // expect: true
System.print(strconv.isFloat("123") == true)  // expect: true
System.print(strconv.isFloat("abc") == false)  // expect: true

// Test formatBool
System.print(strconv.formatBool(true) == "true")  // expect: true
System.print(strconv.formatBool(false) == "false")  // expect: true

// Test module-level convenience functions
System.print(Atoi.call("42") == 42)  // expect: true
System.print(Atof.call("3.14") == 3.14)  // expect: true
System.print(Itoa.call(123) == "123")  // expect: true
System.print(Ftoa.call(2.5).startsWith("2.5"))  // expect: true
