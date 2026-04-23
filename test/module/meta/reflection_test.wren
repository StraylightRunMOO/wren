// Test meta/reflection module functionality

import "meta/reflection" for reflection, ClassInfo, MethodInfo

// Test reflection.getClass with core classes
var listClass = reflection.getClass("List")
System.print(listClass is ClassInfo)             // expect: true
System.print(listClass.name)                     // expect: List
System.print(listClass.hasMethod("add"))         // expect: true
System.print(listClass.hasMethod("nonexistent")) // expect: false

// Test reflection.getClass with non-existent class
var nullClass = reflection.getClass("NonExistentClassXYZ")
System.print(nullClass == null)  // expect: true

// Test reflection.classOf
var myList = [1, 2, 3]
var myListClass = reflection.classOf(myList)
System.print(myListClass.name)  // expect: List

// Test reflection.hasMethod
System.print(reflection.hasMethod(myList, "count"))  // expect: true
System.print(reflection.hasMethod(myList, "zap"))    // expect: false

// Test reflection.hasMethod with null
System.print(reflection.hasMethod(null, "anything"))  // expect: false

// Test custom class
class TestClass {
  construct new() { _field = 42 }
  method1(a, b) { a + b }
  value { _field }
  value=(v) { _field = v }
  static staticMethod() { "static" }
}

var testInfo = reflection.getClass("TestClass")
System.print(testInfo.name)                    // expect: TestClass
System.print(testInfo.hasMethod("method1"))    // expect: true
System.print(testInfo.hasMethod("value"))      // expect: true

// Test arity
System.print(testInfo.arity("method1(_,_)"))  // expect: 2
System.print(testInfo.arity("value"))          // expect: 0

// Test isGetter/isSetter
System.print(testInfo.isGetter("value"))         // expect: true
System.print(testInfo.isSetter("value=(_)"))     // expect: true
System.print(testInfo.isGetter("method1(_,_)"))  // expect: false

// Test isStatic / type
var smInfo = testInfo.methodInfo("staticMethod()")
System.print(smInfo != null)    // expect: true
System.print(smInfo.isStatic)   // expect: true
System.print(smInfo.type)       // expect: block

var m1Info = testInfo.methodInfo("method1(_,_)")
System.print(m1Info.isStatic)   // expect: false
System.print(m1Info.type)       // expect: block

// Test isPrimitive on a built-in
var numInfo = reflection.getClass("Num")
System.print(numInfo != null)   // expect: true
var absInfo = numInfo.methodInfo("abs")
System.print(absInfo != null)   // expect: true
System.print(absInfo.isPrimitive) // expect: true
System.print(absInfo.isStatic)    // expect: false

// Test reflection.fieldsOf — field names not available at runtime
var testInstance = TestClass.new()
var fields = reflection.fieldsOf(testInstance)
System.print(fields is List)    // expect: true

// Test reflection.fieldsOf with null
var fiber1 = Fiber.new { reflection.fieldsOf(null) }
System.print(fiber1.try())  // expect: Cannot get fields of null.
