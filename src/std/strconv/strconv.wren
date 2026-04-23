// strconv - String conversions for Wren
// Convert between strings and basic types

class strconv {
  // Parse integer from string
  static parseInt(s) { parseInt(s, 10) }
  static parseInt(s, base) {
    if (!(s is String)) Fiber.abort("Expected string")
    return parseInt_(s, base)
  }

  // Parse float from string
  static parseFloat(s) {
    if (!(s is String)) Fiber.abort("Expected string")
    return parseFloat_(s)
  }

  // Format integer as string
  static formatInt(n) { formatInt(n, 10) }
  static formatInt(n, base) {
    if (!(n is Num)) Fiber.abort("Expected number")
    if (base < 2 || base > 36) Fiber.abort("Base must be between 2 and 36")
    return formatInt_(n, base)
  }

  // Format float as string
  static formatFloat(n) { formatFloat(n, -1, 6) }
  static formatFloat(n, prec) { formatFloat(n, prec, 6) }
  static formatFloat(n, prec, fmt) {
    if (!(n is Num)) Fiber.abort("Expected number")
    return formatFloat_(n, prec, fmt)
  }

  // Quote a string for use in source code
  static quote(s) { quote_(s) }
  
  // Unquote a string (reverse of quote)
  static unquote(s) { unquote_(s) }

  // Check if string is a valid integer
  static isInt(s) {
    if (!(s is String)) return false
    return isInt_(s)
  }

  // Check if string is a valid float
  static isFloat(s) {
    if (!(s is String)) return false
    return isFloat_(s)
  }

  // Convert bool to string
  static formatBool(b) { b ? "true" : "false" }

  // Append integer to string builder
  static appendInt(builder, n) { appendInt(builder, n, 10) }
  static appendInt(builder, n, base) {
    builder.write(formatInt(n, base))
    return builder
  }

  // Append float to string builder
  static appendFloat(builder, n) { appendFloat(builder, n, -1, 6) }
  static appendFloat(builder, n, prec) { appendFloat(builder, n, prec, 6) }
  static appendFloat(builder, n, prec, fmt) {
    builder.write(formatFloat(n, prec, fmt))
    return builder
  }

  foreign static parseInt_(s, base)
  foreign static parseFloat_(s)
  foreign static formatInt_(n, base)
  foreign static formatFloat_(n, prec, fmt)
  foreign static quote_(s)
  foreign static unquote_(s)
  foreign static isInt_(s)
  foreign static isFloat_(s)
}

// Convenience functions at module level
var Atoi = fn(s) { strconv.parseInt(s) }
var Atof = fn(s) { strconv.parseFloat(s) }
var Itoa = fn(n) { strconv.formatInt(n) }
var Ftoa = fn(n) { strconv.formatFloat(n) }
