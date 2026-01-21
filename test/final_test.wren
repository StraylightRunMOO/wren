System.print("=== All Functional Syntax Features ===")
System.print("")

System.print("1. fn syntax (no parens):")
var f1 = fn { 42 }
System.print("   f1() = %(f1())")

System.print("")
System.print("2. fn syntax (with parens):")
var f2 = fn () { 42 }
System.print("   f2() = %(f2())")

System.print("")
System.print("3. fn syntax (with parameters):")
var add = fn (a, b) { a + b }
System.print("   add(5, 3) = %(add(5, 3))")

System.print("")
System.print("4. Block arguments:")
var list = [1, 2, 3, 4, 5]
var doubled = list.map {|n| n * 2}
System.print("   [1,2,3,4,5].map {|n| n * 2} = %(doubled)")

System.print("")
System.print("5. Inline method calls:")
System.print("   fn {}.arity = %(fn {}.arity)")
System.print("   (fn { 100 })() = %((fn { 100 })())")

System.print("")
System.print("=== ✅ All functional syntax working! ===")
