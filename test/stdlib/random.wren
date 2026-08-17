import "math/random" for Random

var a = Random.new(1)
var b = Random.new(1)
System.print(a.nextUint64() == b.nextUint64())  // expect: true
System.print(a.nextDouble() >= 0)               // expect: true
