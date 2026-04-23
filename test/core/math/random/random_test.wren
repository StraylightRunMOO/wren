// math/random comprehensive tests
import "math/random" for Random, ALG_XOSHIRO256PP, ALG_XOROSHIRO128PP, ALG_WYRAND, ALG_XSHIFT64STAR

// Test deterministic seeding with 42
var r = Random.new(42)

// Test nextUint64 - should produce deterministic values
var u1 = r.nextUint64()
var u2 = r.nextUint64()
var u3 = r.nextUint64()
System.print(u1 > 0)  // expect: true
System.print(u2 > 0)  // expect: true
System.print(u3 > 0)  // expect: true

// Test nextDouble produces values in [0, 1)
var d1 = r.nextDouble()
var d2 = r.nextDouble()
System.print(d1 >= 0 && d1 < 1)  // expect: true
System.print(d2 >= 0 && d2 < 1)  // expect: true

// Test nextFloat
var f1 = r.nextFloat()
System.print(f1 is Num)  // expect: true
System.print(f1 >= 0 && f1 < 1)  // expect: true

// Test nextInt
var i1 = r.nextInt(100)
var i2 = r.nextInt(1000)
System.print(i1 >= 0 && i1 < 100)  // expect: true
System.print(i2 >= 0 && i2 < 1000)  // expect: true

// Test nextIntInRange
var ir1 = r.nextIntInRange(10, 20)
var ir2 = r.nextIntInRange(-50, 50)
System.print(ir1 >= 10 && ir1 <= 20)  // expect: true
System.print(ir2 >= -50 && ir2 <= 50)  // expect: true

// Test nextBool
var bools = [r.nextBool(), r.nextBool(), r.nextBool()]
System.print(bools.count == 3)  // expect: true

// Test choice
var list = ["a", "b", "c", "d", "e"]
var choice = r.choice(list)
System.print(list.contains(choice))  // expect: true

// Test shuffle preserves elements
var toShuffle = [1, 2, 3, 4, 5]
var shuffled = r.shuffle(toShuffle)
System.print(shuffled.count == 5)  // expect: true
System.print(shuffled.contains(1))  // expect: true
System.print(shuffled.contains(2))  // expect: true
System.print(shuffled.contains(3))  // expect: true
System.print(shuffled.contains(4))  // expect: true
System.print(shuffled.contains(5))  // expect: true

// Test sample
var sampleResult = r.sample([1, 2, 3, 4, 5, 6, 7, 8, 9, 10], 5)
System.print(sampleResult.count == 5)  // expect: true

// Test with different algorithms
var r1 = Random.withAlgorithm(ALG_XOROSHIRO128PP, 123)
var r2 = Random.withAlgorithm(ALG_WYRAND, 123)
var r3 = Random.withAlgorithm(ALG_XSHIFT64STAR, 123)
System.print(r1.nextDouble() >= 0)  // expect: true
System.print(r2.nextDouble() >= 0)  // expect: true
System.print(r3.nextDouble() >= 0)  // expect: true

// Test deterministic reproduction with same seed
var rA = Random.new(999)
var rB = Random.new(999)
System.print(rA.nextUint64() == rB.nextUint64())  // expect: true

// Test normal distribution (just verify it produces numbers)
var rNorm = Random.new(42)
var n1 = rNorm.nextNormal()
var n2 = rNorm.nextNormal(10, 2)
System.print(n1 is Num)  // expect: true
System.print(n2 is Num)  // expect: true

// Test exponential distribution
var rexp = Random.new(42)
var exp1 = rexp.nextExponential(1.0)
System.print(exp1 > 0)  // expect: true

// Test poisson distribution  
var rpois = Random.new(42)
var pois1 = rpois.nextPoisson(3.0)
System.print(pois1 is Num)  // expect: true
System.print(pois1 >= 0)  // expect: true
