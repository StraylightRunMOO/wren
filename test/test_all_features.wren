// Test all new functional syntax features

System.print("=== Testing fn syntax ===")

// 1. No parentheses
var f1 = fn { "no parens" }
System.print(f1())

// 2. Empty parentheses
var f2 = fn () { "empty parens" }
System.print(f2())

// 3. With parameters
var add = fn (a, b) { a + b }
System.print("5 + 3 = %(add(5, 3))")

// 4. Inline method calls (precedence fixed)
System.print("Arity of fn {}: %(fn {}.arity)")

// 5. Direct invocation
System.print("Direct call: %((fn { 42 })())")

System.print("\n=== Testing block arguments ===")

var list = [1, 2, 3, 4, 5]
var doubled = list.map {|n| n * 2}
System.print("Doubled: %(doubled.toList)")

var filtered = list.where {|n| n > 3}
System.print("Filtered: %(filtered.toList)")

System.print("\n✅ All functional syntax features working!")
