// math comprehensive tests
import "math" for math, Stats

// Test constants
System.print(math.E > 2.7 && math.E < 2.8)  // expect: true
System.print(math.Pi > 3.14 && math.Pi < 3.15)  // expect: true
System.print(math.Phi > 1.6 && math.Phi < 1.7)  // expect: true
System.print(math.Sqrt2 > 1.4 && math.Sqrt2 < 1.5)  // expect: true

// Test basic functions
System.print(math.abs(-5) == 5)  // expect: true
System.print(math.abs(5) == 5)  // expect: true
System.print(math.ceil(2.3) == 3)  // expect: true
System.print(math.floor(2.9) == 2)  // expect: true
System.print(math.round(2.5) == 3)  // expect: true

// Test exponential and logarithmic
System.print(math.exp(0) == 1)  // expect: true
System.print(math.log(1) == 0)  // expect: true
System.print(math.log10(100) == 2)  // expect: true
System.print(math.log2(8) == 3)  // expect: true

// Test power
System.print(math.pow(2, 3) == 8)  // expect: true
System.print(math.pow(3, 2) == 9)  // expect: true

// Test trigonometric
System.print(math.sin(0) == 0)  // expect: true
System.print(math.cos(0) == 1)  // expect: true
System.print(math.sin(math.Pi / 2).round == 1)  // expect: true

// Test atan2
System.print(math.atan2(1, 1) == math.Pi / 4)  // expect: true

// Test hyperbolic
System.print(math.sinh(0) == 0)  // expect: true
System.print(math.cosh(0) == 1)  // expect: true

// Test hypot
System.print(math.hypot(3, 4) == 5)  // expect: true
System.print(math.hypot(5, 12) == 13)  // expect: true

// Test sign
System.print(math.sign(5) == 1)  // expect: true
System.print(math.sign(-5) == -1)  // expect: true
System.print(math.sign(0) == 0)  // expect: true

// Test clamp
System.print(math.clamp(5, 0, 10) == 5)  // expect: true
System.print(math.clamp(-5, 0, 10) == 0)  // expect: true
System.print(math.clamp(15, 0, 10) == 10)  // expect: true

// Test lerp
System.print(math.lerp(0, 10, 0) == 0)  // expect: true
System.print(math.lerp(0, 10, 0.5) == 5)  // expect: true
System.print(math.lerp(0, 10, 1) == 10)  // expect: true

// Test smoothstep
System.print(math.smoothstep(0, 1, 0) == 0)  // expect: true
System.print(math.smoothstep(0, 1, 1) == 1)  // expect: true

// Test isPow2
System.print(math.isPow2(1) == true)  // expect: true
System.print(math.isPow2(2) == true)  // expect: true
System.print(math.isPow2(4) == true)  // expect: true
System.print(math.isPow2(8) == true)  // expect: true
System.print(math.isPow2(3) == false)  // expect: true
System.print(math.isPow2(5) == false)  // expect: true

// Test nextPow2
System.print(math.nextPow2(1) == 1)  // expect: true
System.print(math.nextPow2(2) == 2)  // expect: true
System.print(math.nextPow2(3) == 4)  // expect: true
System.print(math.nextPow2(5) == 8)  // expect: true

// Test bit manipulation
System.print(math.bitAnd(3, 1) == 1)  // expect: true
System.print(math.bitOr(2, 1) == 3)  // expect: true
System.print(math.bitXor(3, 1) == 2)  // expect: true
System.print(math.bitShiftLeft(1, 3) == 8)  // expect: true
System.print(math.bitShiftRight(8, 3) == 1)  // expect: true

// Test floating point utilities
System.print(math.isInf(1/0) == true)  // expect: true
System.print(math.isInf(0) == false)  // expect: true
System.print(math.isNaN(0/0) == true)  // expect: true
System.print(math.isNaN(0) == false)  // expect: true
System.print(math.isFinite(5) == true)  // expect: true
System.print(math.isFinite(1/0) == false)  // expect: true

// Test approxEqual
System.print(math.approxEqual(1.0, 1.000001) == true)  // expect: true
System.print(math.approxEqual(1.0, 2.0) == false)  // expect: true

// Test Stats
var data = [1, 2, 3, 4, 5]
System.print(Stats.sum(data) == 15)  // expect: true
System.print(Stats.mean(data) == 3)  // expect: true
System.print(Stats.min(data) == 1)  // expect: true
System.print(Stats.max(data) == 5)  // expect: true

// Test Stats variance/stddev
var varData = [2, 4, 4, 4, 5, 5, 7, 9]
var variance = Stats.variance(varData)
var stddev = Stats.stddev(varData)
System.print(variance > 0)  // expect: true
System.print(stddev > 0)  // expect: true

// Test Stats median
System.print(Stats.median([1, 2, 3, 4, 5]) == 3)  // expect: true
System.print(Stats.median([1, 2, 3, 4]) == 2.5)  // expect: true

// Test Stats percentile
var sorted = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
System.print(Stats.percentile(sorted, 0) == 1)  // expect: true
System.print(Stats.percentile(sorted, 50) == 6)  // expect: true
System.print(Stats.percentile(sorted, 100) == 10)  // expect: true
