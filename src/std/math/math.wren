// math - Mathematical functions for Wren
// Extends core Num functionality with advanced math operations

class math {
  // Constants
  static E { 2.71828182845904523536028747135266250 }
  static Pi { 3.14159265358979323846264338327950288 }
  static Phi { 1.61803398874989484820458683436563811 }  // Golden ratio
  static Sqrt2 { 1.41421356237309504880168872420969808 }
  static Ln2 { 0.693147180559945309417232121458176568 }
  static Ln10 { 2.30258509299404568401799145468436421 }
  
  // Basic functions (wrappers around Num methods)
  static abs(x) { x.abs }
  static ceil(x) { x.ceil }
  static floor(x) { x.floor }
  static round(x) { x.round }
  static trunc(x) { x.trunc }
  static sqrt(x) { x.sqrt }
  static cbrt(x) { x.cbrt }
  
  // Exponential and logarithmic
  static exp(x) { x.exp }
  static exp2(x) { x.pow(2) }
  static log(x) { x.log }
  static log10(x) { x.log / this.Ln10 }
  static log2(x) { x.log / this.Ln2 }
  static log1p(x) { (1 + x).log }
  
  // Power
  static pow(x, y) { x.pow(y) }
  
  // Trigonometric
  static sin(x) { x.sin }
  static cos(x) { x.cos }
  static tan(x) { x.tan }
  
  // Inverse trigonometric
  static asin(x) { x.asin }
  static acos(x) { x.acos }
  static atan(x) { x.atan }
  static atan2(y, x) {
    if (x > 0) {
      return (y / x).atan
    } else if (x < 0) {
      if (y >= 0) {
        return (y / x).atan + Pi
      } else {
        return (y / x).atan - Pi
      }
    } else {
      // x == 0
      if (y > 0) return Pi / 2
      if (y < 0) return -Pi / 2
      return 0
    }
  }
  
  // Hyperbolic
  static sinh(x) { (exp(x) - exp(-x)) / 2 }
  static cosh(x) { (exp(x) + exp(-x)) / 2 }
  static tanh(x) { sinh(x) / cosh(x) }
  
  // Inverse hyperbolic
  static asinh(x) { (x + sqrt(x * x + 1)).log }
  static acosh(x) { (x + sqrt(x * x - 1)).log }
  static atanh(x) { ((1 + x) / (1 - x)).log / 2 }
  
  // Special
  static hypot(x, y) { sqrt(x * x + y * y) }
  static gamma(x) { foreignGamma_(x) }
  static lgamma(x) { foreignLgamma_(x) }
  static erf(x) { foreignErf_(x) }
  static erfc(x) { 1 - erf(x) }
  
  // Sign function
  static sign(x) {
    if (x > 0) return 1
    if (x < 0) return -1
    return 0
  }
  
  // Clamp value between min and max
  static clamp(x, min, max) {
    if (x < min) return min
    if (x > max) return max
    return x
  }
  
  // Linear interpolation
  static lerp(a, b, t) { a + (b - a) * t }
  
  // Smoothstep interpolation
  static smoothstep(edge0, edge1, x) {
    var t = clamp((x - edge0) / (edge1 - edge0), 0, 1)
    return t * t * (3 - 2 * t)
  }
  
  // Smootherstep interpolation
  static smootherstep(edge0, edge1, x) {
    var t = clamp((x - edge0) / (edge1 - edge0), 0, 1)
    return t * t * t * (t * (t * 6 - 15) + 10)
  }
  
  // Remap value from one range to another
  static remap(x, inMin, inMax, outMin, outMax) {
    return outMin + (x - inMin) * (outMax - outMin) / (inMax - inMin)
  }
  
  // Check if value is power of 2
  static isPow2(x) { x > 0 && (x & (x - 1)) == 0 }
  
  // Next power of 2
  static nextPow2(x) {
    if (x <= 1) return 1
    x = x - 1
    x = x | (x >> 1)
    x = x | (x >> 2)
    x = x | (x >> 4)
    x = x | (x >> 8)
    x = x | (x >> 16)
    return x + 1
  }
  
  // Bit manipulation
  static bitNot(x) { ~x }
  static bitAnd(x, y) { x & y }
  static bitOr(x, y) { x | y }
  static bitXor(x, y) { x ^ y }
  static bitShiftLeft(x, y) { x << y }
  static bitShiftRight(x, y) { x >> y }
  static bitRotateLeft(x, k) { (x << k) | (x >> (32 - k)) }
  static bitRotateRight(x, k) { (x >> k) | (x << (32 - k)) }
  
  // Floating point utilities
  static isInf(x) { x == 1/0 || x == -1/0 }
  static isNaN(x) { x != x }
  static isFinite(x) { !isInf(x) && !isNaN(x) }
  
  // Approximately equal (for floating point comparison)
  static approxEqual(a, b) { approxEqual(a, b, 0.00001) }
  static approxEqual(a, b, epsilon) { (a - b).abs <= epsilon }
  
  // Foreign methods for special functions
  foreign static foreignGamma_(x)
  foreign static foreignLgamma_(x)
  foreign static foreignErf_(x)
}

// Random number generator with more features than core Random
class Rand {
  construct new(seed) {
    _seed = seed
    _state = seed
  }
  
  construct new() {
    _seed = systemTime_()
    _state = _seed
  }
  
  foreign static systemTime_()
  
  // Linear congruential generator
  next() {
    _state = (_state * 1103515245 + 12345) % 2147483648
    return _state / 2147483648
  }
  
  // Random integer in range [0, n)
  int(n) { (next() * n).floor }
  
  // Random float in range [0, 1)
  float() { next() }
  
  // Random float in range [min, max)
  range(min, max) { min + next() * (max - min) }
  
  // Random boolean
  bool() { next() >= 0.5 }
  
  // Random choice from list
  choice(list) { list[int(list.count)] }
  
  // Shuffle list in place
  shuffle(list) {
    var n = list.count
    while (n > 1) {
      n = n - 1
      var k = int(n + 1)
      var temp = list[n]
      list[n] = list[k]
      list[k] = temp
    }
    return list
  }
  
  // Normal distribution (Box-Muller transform)
  normal() { normal(0, 1) }
  normal(mean, stddev) {
    var u1 = next()
    var u2 = next()
    var z0 = sqrt(-2 * u1.log) * (2 * math.Pi * u2).cos
    return mean + z0 * stddev
  }
  
  seed { _seed }
  
  reset() {
    _state = _seed
    return this
  }
}

// Statistical functions
class Stats {
  static sum(list) { list.reduce(0) { |acc, x| acc + x } }
  static mean(list) { sum(list) / list.count }
  static min(list) { list.reduce(list[0]) { |m, x| x < m ? x : m } }
  static max(list) { list.reduce(list[0]) { |m, x| x > m ? x : m } }
  
  static variance(list) {
    var m = mean(list)
    return list.map { |x| (x - m) * (x - m) }.reduce(0) { |acc, x| acc + x } / list.count
  }
  
  static stddev(list) { variance(list).sqrt }
  
  static median(list) {
    var sorted = list.toList.sort()
    var n = sorted.count
    if (n % 2 == 1) {
      return sorted[(n / 2).floor]
    }
    return (sorted[n / 2 - 1] + sorted[n / 2]) / 2
  }
  
  static percentile(list, p) {
    var sorted = list.toList.sort()
    var idx = (p / 100 * (sorted.count - 1)).round
    return sorted[idx]
  }
}

// Constants at module level (for convenience)
var Inf = 1 / 0
var NaN = 0 / 0
var E = math.E
var Pi = math.Pi
var Phi = math.Phi
