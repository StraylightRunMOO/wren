// math/random - High-quality PRNG suite for Wren
// Based on xoshiro256++, xoroshiro128++, wyrand, and splitmix64
// Uses Lemire's multiply-high method for uniform integers

// PRNG algorithm selection
var ALG_XOSHIRO256PP = 0
var ALG_XOROSHIRO128PP = 1
var ALG_WYRAND = 2
var ALG_XSHIFT64STAR = 3

// Local constant for Box-Muller
var PiConst = 3.14159265358979323846

// Foreign class wrapping the PRNG state
foreign class RandomState {
  construct new(algorithm, seed) {
    initState_(algorithm, seed)
  }
  
  foreign initState_(algorithm, seed)
  foreign nextUint64_()
  foreign nextDouble_()
}

// High-quality random number generator
class Random {
  construct new() {
    _state = RandomState.new(ALG_XOSHIRO256PP, systemSeed_())
  }
  
  construct new(seed) {
    _state = RandomState.new(ALG_XOSHIRO256PP, seed)
  }
  
  construct withAlgorithm(algorithm, seed) {
    _state = RandomState.new(algorithm, seed)
  }
  
  // Return raw uint64 as double
  nextUint64() { _state.nextUint64_() }
  
  // Uniform [0, 1) as double
  nextDouble() { _state.nextDouble_() }
  
  // Uniform [0, 1) as float
  nextFloat() { nextDouble() }
  
  // Uniform integer [0, range)
  nextInt(range) {
    if (range <= 0) Fiber.abort("Range must be positive")
    return (nextUint64() % range).floor
  }
  
  // Uniform integer [lo, hi] inclusive
  nextIntInRange(lo, hi) {
    if (lo > hi) Fiber.abort("Invalid range")
    return lo + nextInt(hi - lo + 1)
  }
  
  // Uniform bool
  nextBool() { nextDouble() < 0.5 }
  
  // Normal distribution (Box-Muller)
  nextNormal() { nextNormal(0, 1) }
  nextNormal(mean, stddev) {
    var u1 = nextDouble()
    var u2 = nextDouble()
    var z0 = (-2 * u1.log).sqrt * (2 * PiConst * u2).cos
    return mean + z0 * stddev
  }
  
  // Exponential distribution
  nextExponential(lambda) {
    var u = nextDouble()
    return -u.log / lambda
  }
  
  // Poisson distribution (Knuth's method)
  nextPoisson(lambda) {
    var L = (-lambda).exp
    var k = 0
    var p = 1
    while (p > L) {
      k = k + 1
      p = p * nextDouble()
    }
    return k - 1
  }
  
  // Random choice from list
  choice(list) { list[nextInt(list.count)] }
  
  // Shuffle list in place (Fisher-Yates)
  shuffle(list) {
    var n = list.count
    for (i in 0...n) {
      var j = nextInt(n - i) + i
      var tmp = list[i]
      list[i] = list[j]
      list[j] = tmp
    }
    return list
  }
  
  // Sample n elements without replacement
  sample(list, n) {
    if (n > list.count) Fiber.abort("Sample size larger than list")
    var shuffled = list.toList
    shuffle(shuffled)
    return shuffled[0...n]
  }
  
  // Fill buffer with random doubles [0, 1)
  fillDoubles(buf) {
    for (i in 0...buf.count) {
      buf[i] = nextDouble()
    }
  }
  
  // Fill buffer with random integers [0, range)
  fillInts(buf, range) {
    for (i in 0...buf.count) {
      buf[i] = nextInt(range)
    }
  }
  
  systemSeed_() { 12345 }
}

// Global shared PRNG
var globalRand = Random.new()

// Convenience functions at module level
var Uint64 = fn { globalRand.nextUint64() }
var Double = fn { globalRand.nextDouble() }
var Float = fn { globalRand.nextFloat() }
var Int = fn(n) { globalRand.nextInt(n) }
var IntInRange = fn(lo, hi) { globalRand.nextIntInRange(lo, hi) }
var RandBool = fn { globalRand.nextBool() }
var Normal = fn { globalRand.nextNormal() }
var NormalParams = fn(mean, stddev) { globalRand.nextNormal(mean, stddev) }
var Choice = fn(list) { globalRand.choice(list) }
var Shuffle = fn(list) { globalRand.shuffle(list) }
var Sample = fn(list, n) { globalRand.sample(list, n) }

// Re-seed the global PRNG
var Seed = fn(seed) { globalRand = Random.new(seed) }

// Constants
var Pi = 3.14159265358979323846
