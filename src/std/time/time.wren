// time - Time and date operations for Wren

class Time {
  // Get current Unix timestamp in seconds
  foreign static now()
  
  // Get current time in milliseconds
  foreign static nowMillis()
  
  // Get current time in nanoseconds
  foreign static nowNanos()
  
  // Sleep for specified seconds (can be fractional)
  foreign static sleep(seconds)
  
  // Parse time from string
  construct parse(layout, value) {
    _sec = Time.parse_(layout, value)
  }
  
  // Create time from Unix timestamp
  construct fromUnix(sec) {
    _sec = sec
  }
  
  // Create time from components (year, month, day, hour, min, sec)
  construct new(year, month, day, hour, min, sec) {
    _sec = Time.fromComponents_(year, month, day, hour, min, sec)
  }
  
  // Getters
  sec { _sec }
  year { Time.getComponent_(_sec, 0) }
  month { Time.getComponent_(_sec, 1) }
  day { Time.getComponent_(_sec, 2) }
  hour { Time.getComponent_(_sec, 3) }
  minute { Time.getComponent_(_sec, 4) }
  second { Time.getComponent_(_sec, 5) }
  weekday { Time.getComponent_(_sec, 6) }
  yearday { Time.getComponent_(_sec, 7) }
  
  // Format time according to layout
  format(layout) { Time.format_(_sec, layout) }
  
  // Add duration
  add(d) { Time.fromUnix(_sec + d.seconds) }
  
  // Subtract duration or time
  sub(other) {
    if (other is Time) {
      return Duration.new(_sec - other.sec)
    }
    return Time.fromUnix(_sec - other.seconds)
  }
  
  // Comparison
  <(other) { _sec < other.sec }
  >(other) { _sec > other.sec }
  ==(other) { _sec == other.sec }
  <=(other) { _sec <= other.sec }
  >=(other) { _sec >= other.sec }
  
  toString { format("2006-01-02 15:04:05") }
  
  foreign static parse_(layout, value)
  foreign static fromComponents_(year, month, day, hour, min, sec)
  foreign static getComponent_(sec, idx)
  foreign static format_(sec, layout)
}

// Duration represents elapsed time
class Duration {
  construct new(seconds) {
    _sec = seconds
  }
  
  // Common durations
  static zero { Duration.new(0) }
  static second { Duration.new(1) }
  static minute { Duration.new(60) }
  static hour { Duration.new(3600) }
  static day { Duration.new(86400) }
  static week { Duration.new(604800) }
  
  // Factory methods
  static hours(n) { Duration.new(n * 3600) }
  static minutes(n) { Duration.new(n * 60) }
  static seconds(n) { Duration.new(n) }
  static milliseconds(n) { Duration.new(n / 1000) }
  static microseconds(n) { Duration.new(n / 1000000) }
  static nanoseconds(n) { Duration.new(n / 1000000000) }
  
  seconds { _sec }
  milliseconds { (_sec * 1000).round }
  microseconds { (_sec * 1000000).round }
  nanoseconds { (_sec * 1000000000).round }
  
  hours { _sec / 3600 }
  mins { (_sec % 3600) / 60 }
  secs { _sec % 60 }
  
  // Arithmetic
  +(other) { Duration.new(_sec + other.seconds) }
  -(other) { Duration.new(_sec - other.seconds) }
  *(n) { Duration.new(_sec * n) }
  /(n) { Duration.new(_sec / n) }
  
  // Comparison
  <(other) { _sec < other.seconds }
  >(other) { _sec > other.seconds }
  ==(other) { _sec == other.seconds }
  
  abs { Duration.new(_sec.abs) }
  
  toString { 
    var h = hours.floor
    var m = mins.floor
    var s = secs
    if (h > 0) {
      return "%(h)h%(m)m%(s)s"
    } else if (m > 0) {
      return "%(m)m%(s)s"
    }
    return "%(s)s"
  }
}

// Timer for benchmarking
class Timer {
  construct new() {
    _start = null
    _elapsed = 0
  }
  
  start() {
    _start = Time.now()
    return this
  }
  
  stop() {
    if (_start != null) {
      _elapsed = Time.now() - _start
      _start = null
    }
    return this
  }
  
  reset() {
    _start = null
    _elapsed = 0
    return this
  }
  
  elapsed { _start != null ? Time.now() - _start : _elapsed }
  
  lap() {
    var now = Time.now()
    var lapTime = now - (_start != null ? _start : now)
    _start = now
    return lapTime
  }
}

// Convenience function
var Now = fn { Time.now() }
var Sleep = fn(s) { Time.sleep(s) }
