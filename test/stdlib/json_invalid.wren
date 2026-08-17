import "encoding/json" for Json
Json.parse("1 2")  // expect runtime error: Trailing data after JSON value
