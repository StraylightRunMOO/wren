// strings - String utilities for Wren
// Go-like string manipulation functions

class strings {
  // Contains reports whether substr is within s
  static contains(s, substr) { s.contains(substr) }
  
  // ContainsAny reports whether any Unicode code points in chars are within s
  static containsAny(s, chars) {
    for (c in chars) {
      if (s.contains(c)) return true
    }
    return false
  }
  
  // Count counts the number of non-overlapping instances of substr in s
  static count(s, substr) {
    if (substr == "") return s.count + 1
    var count = 0
    var idx = 0
    while ((idx = s.indexOf(substr, idx)) != -1) {
      count = count + 1
      idx = idx + substr.count
    }
    return count
  }
  
  // Fields splits the string s around each instance of one or more consecutive white space
  static fields(s) {
    var result = []
    var field = ""
    for (c in s) {
      if (c == " " || c == "\t" || c == "\n" || c == "\r") {
        if (field != "") {
          result.add(field)
          field = ""
        }
      } else {
        field = field + c
      }
    }
    if (field != "") result.add(field)
    return result
  }
  
  // HasPrefix tests whether the string s begins with prefix
  static hasPrefix(s, prefix) { s.startsWith(prefix) }
  
  // HasSuffix tests whether the string s ends with suffix
  static hasSuffix(s, suffix) { s.endsWith(suffix) }
  
  // Index returns the index of the first instance of substr in s, or -1 if not present
  static index(s, substr) { s.indexOf(substr) }
  
  // IndexAny returns the index of the first instance of any Unicode code point from chars
  static indexAny(s, chars) {
    for (i in 0...s.count) {
      if (chars.contains(s[i])) return i
    }
    return -1
  }
  
  // Join concatenates the elements of a to create a single string
  static join(a) { join(a, "") }
  static join(a, sep) {
    var result = ""
    var first = true
    for (s in a) {
      if (!first) result = result + sep
      result = result + s.toString
      first = false
    }
    return result
  }
  
  // LastIndex returns the index of the last instance of substr in s, or -1 if not present
  static lastIndex(s, substr) { s.indexOf(substr) } // TODO: implement reverse search
  
  // Map returns a copy of the string s with all its characters modified according to the mapping function
  static map(s, mapper) {
    var result = ""
    for (c in s) {
      result = result + mapper.call(c)
    }
    return result
  }
  
  // Repeat returns a new string consisting of count copies of the string s
  static repeat(s, count) {
    var result = ""
    for (i in 0...count) {
      result = result + s
    }
    return result
  }
  
  // Replace returns a copy of the string s with the first n non-overlapping instances of old replaced by new
  static replace(s, old, new) { replace(s, old, new, -1) }
  static replace(s, old, new, n) {
    if (old == "") return s
    if (n == 0) return s
    
    var result = ""
    var idx = 0
    var count = 0
    var lastIdx = 0
    
    while (lastIdx < s.count && (idx = s.indexOf(old, lastIdx)) != -1) {
      if (n > 0 && count >= n) break
      result = result + s[lastIdx...idx] + new
      lastIdx = idx + old.count
      count = count + 1
    }
    if (lastIdx < s.count) {
      result = result + s[lastIdx..-1]
    }
    return result
  }
  
  // Split slices s into all substrings separated by sep and returns a slice of the substrings
  static split(s) { split(s, "", -1) }
  static split(s, sep) { split(s, sep, -1) }
  static split(s, sep, n) {
    if (sep == "") {
      // Split into characters
      var result = []
      for (c in s) result.add(c)
      return result
    }
    
    var result = []
    var idx = 0
    var count = 0
    var lastIdx = 0
    
    while ((idx = s.indexOf(sep, lastIdx)) != -1) {
      if (n > 0 && count >= n - 1) break
      result.add(s[lastIdx...idx])
      lastIdx = idx + sep.count
      count = count + 1
    }
    result.add(s[lastIdx..-1])
    return result
  }
  
  // SplitAfter slices s into all substrings after each instance of sep
  static splitAfter(s, sep) {
    var result = []
    var idx = 0
    var lastIdx = 0
    
    while ((idx = s.indexOf(sep, lastIdx)) != -1) {
      result.add(s[lastIdx..idx + sep.count - 1])
      lastIdx = idx + sep.count
    }
    if (lastIdx < s.count) {
      result.add(s[lastIdx..-1])
    }
    return result
  }
  
  // Title returns a copy of the string s with all Unicode letters that begin words mapped to their title case
  static title(s) {
    var result = ""
    var newWord = true
    for (c in s) {
      if (c == " " || c == "\t" || c == "\n") {
        newWord = true
        result = result + c
      } else if (newWord) {
        // Uppercase first letter of word
        result = result + c.toString[0].bytes.map(fn(b) { b >= 97 && b <= 122 ? String.fromByte(b - 32) : String.fromByte(b) }).join()
        newWord = false
      } else {
        result = result + c
      }
    }
    return result
  }
  
  // ToLower returns a copy of the string s with all Unicode letters mapped to their lower case
  static toLower(s) { s.bytes.map(fn(b) { b >= 65 && b <= 90 ? String.fromByte(b + 32) : String.fromByte(b) }).join() }
  
  // ToUpper returns a copy of the string s with all Unicode letters mapped to their upper case
  static toUpper(s) { s.bytes.map(fn(b) { b >= 97 && b <= 122 ? String.fromByte(b - 32) : String.fromByte(b) }).join() }
  
  // Trim returns a slice of the string s with all leading and trailing Unicode code points contained in cutset removed
  static trim(s) { trim(s, " \t\n\r") }
  static trim(s, cutset) {
    return trimRight(trimLeft(s, cutset), cutset)
  }
  
  // TrimLeft returns a slice of the string s with all leading Unicode code points contained in cutset removed
  static trimLeft(s) { trimLeft(s, " \t\n\r") }
  static trimLeft(s, cutset) {
    var start = 0
    while (start < s.count && cutset.contains(s[start])) {
      start = start + 1
    }
    return s[start..-1]
  }
  
  // TrimRight returns a slice of the string s with all trailing Unicode code points contained in cutset removed
  static trimRight(s) { trimRight(s, " \t\n\r") }
  static trimRight(s, cutset) {
    var end = s.count - 1
    while (end >= 0 && cutset.contains(s[end])) {
      end = end - 1
    }
    return s[0..end]
  }
  
  // TrimPrefix returns s without the provided leading prefix string
  static trimPrefix(s, prefix) {
    if (s.startsWith(prefix)) {
      return s[prefix.count..-1]
    }
    return s
  }
  
  // TrimSuffix returns s without the provided trailing suffix string
  static trimSuffix(s, suffix) {
    if (s.endsWith(suffix)) {
      return s[0..-(suffix.count + 1)]
    }
    return s
  }
}

// Builder for efficient string construction
class Builder {
  construct new() {
    _parts = []
  }
  
  write(s) {
    _parts.add(s.toString)
    return this
  }
  
  writeln(s) {
    _parts.add(s.toString)
    _parts.add("\n")
    return this
  }
  
  writeByte(b) {
    _parts.add(String.fromByte(b))
    return this
  }
  
  len { _parts.reduce(0) { |acc, s| acc + s.count } }
  
  reset() {
    _parts = []
    return this
  }
  
  toString {
    var result = ""
    for (part in _parts) {
      result = result + part
    }
    return result
  }
}

// Convenience functions
var Contains = fn(s, substr) { strings.contains(s, substr) }
var HasPrefix = fn(s, prefix) { strings.hasPrefix(s, prefix) }
var HasSuffix = fn(s, suffix) { strings.hasSuffix(s, suffix) }
var Join = fn(a, sep) { strings.join(a, sep) }
var Repeat = fn(s, n) { strings.repeat(s, n) }
var Replace = fn(s, old, new) { strings.replace(s, old, new) }
var Split = fn(s, sep) { strings.split(s, sep) }
var Trim = fn(s) { strings.trim(s) }
var ToLower = fn(s) { strings.toLower(s) }
var ToUpper = fn(s) { strings.toUpper(s) }
