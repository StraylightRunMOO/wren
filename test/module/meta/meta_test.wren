// Test meta module functionality

import "meta" for meta

// Test meta.eval - basic expression
meta.eval("var eval_test = 42")

// Test meta.eval - function definition
meta.eval("var eval_fn = fn(x) { x * 2 }")

// Test meta.eval - class definition
meta.eval("class EvalClass { static greet { \"Hello\" } }")

// Verify eval worked by looking at the current module's variables
var mainVars = meta.getModuleVariables("./test/module/meta/meta_test")
System.print(mainVars.contains("eval_test"))  // expect: true
System.print(mainVars.contains("eval_fn"))    // expect: true
System.print(mainVars.contains("EvalClass"))  // expect: true

// Test meta.compileExpression - single expression, returns callable fn
var expr = meta.compileExpression("5 + 3")
System.print(expr.call())  // expect: 8

// Test meta.getModuleVariables on meta module
var vars = meta.getModuleVariables("meta")
System.print(vars is List)           // expect: true
System.print(vars.contains("meta"))  // expect: true

// Test error: non-string source
var fiber1 = Fiber.new { meta.eval(123) }
System.print(fiber1.try())  // expect: Source code must be a string.

// Test error: invalid module name type
var fiber2 = Fiber.new { meta.getModuleVariables(123) }
System.print(fiber2.try())  // expect: Module name must be a string.

// Test error: non-existent module
var fiber3 = Fiber.new { meta.getModuleVariables("nonexistent_module_xyz") }
System.print(fiber3.try())  // expect: Could not find a module named 'nonexistent_module_xyz'.

System.print("Meta tests passed!")  // expect: Meta tests passed!
