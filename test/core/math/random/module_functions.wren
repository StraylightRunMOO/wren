// Test module-level convenience functions
import "math/random" for Double, Int, RandBool, Choice, Shuffle, Seed

// Seed for deterministic tests
Seed.call(42)

// Test Double
var d1 = Double.call()
var d2 = Double.call()
System.print(d1 >= 0 && d1 < 1)  // expect: true
System.print(d2 >= 0 && d2 < 1)  // expect: true

// Test Int
var i1 = Int.call(100)
var i2 = Int.call(10)
System.print(i1 >= 0 && i1 < 100)  // expect: true
System.print(i2 >= 0 && i2 < 10)  // expect: true

// Test RandBool
var b1 = RandBool.call()
var b2 = RandBool.call()
System.print(b1 is Bool)  // expect: true
System.print(b2 is Bool)  // expect: true

// Test Choice
var list = ["x", "y", "z"]
var c = Choice.call(list)
System.print(list.contains(c))  // expect: true

// Test Shuffle
var s = Shuffle.call([1, 2, 3])
System.print(s.count == 3)  // expect: true
System.print(s.contains(1) && s.contains(2) && s.contains(3))  // expect: true
