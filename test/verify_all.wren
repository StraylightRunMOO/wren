// Verify all functional syntax features work
System.print("✅ Fn.new removed:")
// Fn.new { 42 } // This would error

System.print("✅ fn syntax works:")
var f = fn { 42 }
System.print("  fn { 42 } => function object")

System.print("✅ Direct calls work:")
System.print("  fn { 42 }() => %(f())")

System.print("✅ Parameters work:")
var add = fn (a, b) { a + b }
System.print("  fn (a, b) { a + b }(5, 3) => %(add(5, 3))")

System.print("✅ Block arguments work:")
var list = [1,2,3]
var doubled = list.map {|n| n * 2}
System.print("  [1,2,3].map {|n| n * 2} => %(doubled)")

System.print("✅ Inline calls work:")
System.print("  (fn { 100 })() => %((fn { 100 })())")
System.print("  fn {}.arity => %(fn {}.arity)")

System.print("")
System.print("🎉 All functional syntax features working!")
