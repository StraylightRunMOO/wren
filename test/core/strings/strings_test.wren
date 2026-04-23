// strings comprehensive tests
import "strings" for strings, Builder

// Test contains
System.print(strings.contains("hello world", "world") == true)  // expect: true
System.print(strings.contains("hello world", "foo") == false)  // expect: true

// Test containsAny
System.print(strings.containsAny("hello", "aeiou") == true)  // expect: true
System.print(strings.containsAny("xyz", "aeiou") == false)  // expect: true

// Test count
System.print(strings.count("hello", "l") == 2)  // expect: true
System.print(strings.count("hello", "ll") == 1)  // expect: true
System.print(strings.count("hello", "x") == 0)  // expect: true

// Test fields
var fields = strings.fields("  hello   world  ")
System.print(fields.count == 2)  // expect: true
System.print(fields[0] == "hello")  // expect: true
System.print(fields[1] == "world")  // expect: true

// Test hasPrefix/hasSuffix
System.print(strings.hasPrefix("hello", "he") == true)  // expect: true
System.print(strings.hasPrefix("hello", "lo") == false)  // expect: true
System.print(strings.hasSuffix("hello", "lo") == true)  // expect: true
System.print(strings.hasSuffix("hello", "he") == false)  // expect: true

// Test index
System.print(strings.index("hello", "l") == 2)  // expect: true
System.print(strings.index("hello", "x") == -1)  // expect: true

// Test join
System.print(strings.join(["a", "b", "c"]) == "abc")  // expect: true
System.print(strings.join(["a", "b", "c"], ",") == "a,b,c")  // expect: true

// Test repeat
System.print(strings.repeat("ab", 3) == "ababab")  // expect: true
System.print(strings.repeat("x", 0) == "")  // expect: true

// Test replace
System.print(strings.replace("hello world", "world", "universe") == "hello universe")  // expect: true
System.print(strings.replace("foo foo foo", "foo", "bar", 2) == "bar bar foo")  // expect: true

// Test split
var split1 = strings.split("a,b,c", ",")
System.print(split1.count == 3)  // expect: true
System.print(split1[0] == "a")  // expect: true
System.print(split1[1] == "b")  // expect: true
System.print(split1[2] == "c")  // expect: true

// Test split with limit
var split2 = strings.split("a,b,c,d", ",", 2)
System.print(split2.count == 2)  // expect: true

// Test split (character split)
var split3 = strings.split("abc")
System.print(split3.count == 3)  // expect: true

// Test trim
System.print(strings.trim("  hello  ") == "hello")  // expect: true
System.print(strings.trim("...hello...", ".") == "hello")  // expect: true

// Test trimLeft/trimRight
System.print(strings.trimLeft("  hello  ") == "hello  ")  // expect: true
System.print(strings.trimRight("  hello  ") == "  hello")  // expect: true

// Test trimPrefix/trimSuffix
System.print(strings.trimPrefix("hello world", "hello ") == "world")  // expect: true
System.print(strings.trimSuffix("hello world", " world") == "hello")  // expect: true

// Test toLower/toUpper
System.print(strings.toLower("HELLO") == "hello")  // expect: true
System.print(strings.toUpper("hello") == "HELLO")  // expect: true

// Test Builder
var b = Builder.new()
b.write("hello")
b.write(" ")
b.writeln("world")
b.writeByte(33)  // '!'
System.print(b.toString == "hello world\n!")  // expect: true
b.reset()
System.print(b.toString == "")  // expect: true
