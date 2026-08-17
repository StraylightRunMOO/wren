// encoding/json — JSON parse / encode.
// parse_ is a C recursive-descent parser that builds List/Map via the VM API.
// encode walks Wren values; string and number formatting live in C.

class Json {
  // Expression-body null. `return null` from this class's statement methods
  // comes back as the receiver; Json.nil is the reliable JSON null value.
  static nil { null }

  static parse(text) {
    var value = parse_(text)
    if (value is Bool) return value
    if (value is Num) return value
    if (value is String) return value
    if (value is List) return value
    if (value is Map) return value
    return Json.nil
  }

  static encode(value) { encodeValue_(value) }

  static encodeValue_(v) {
    if (v == null) return "null"
    if (v is Bool) return v ? "true" : "false"
    if (v is Num) return encodeNum_(v)
    if (v is String) return encodeString_(v)
    if (v is List) {
      var parts = []
      for (e in v) parts.add(encodeValue_(e))
      return "[" + parts.join(",") + "]"
    }
    if (v is Map) {
      var parts = []
      for (entry in v) {
        if (!(entry.key is String)) {
          Fiber.abort("JSON object keys must be strings")
        }
        parts.add(encodeString_(entry.key) + ":" + encodeValue_(entry.value))
      }
      return "{" + parts.join(",") + "}"
    }
    Fiber.abort("Cannot encode %(v.type) as JSON")
  }

  foreign static parse_(text)
  foreign static encodeNum_(n)
  foreign static encodeString_(s)
}
