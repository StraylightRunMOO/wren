import "encoding/json" for Json

System.print(Json.parse("null") == null)     // expect: true
System.print(Json.parse("true") == true)     // expect: true
System.print(Json.parse("false") == false)   // expect: true
System.print(Json.parse("42") == 42)         // expect: true
System.print(Json.parse("-1.5") == -1.5)     // expect: true
System.print(Json.parse("\"hi\"") == "hi")   // expect: true

var arr = Json.parse("[1,2,3]")
System.print(arr.count == 3)                 // expect: true
System.print(arr[1] == 2)                    // expect: true

var obj = Json.parse("{\"a\":1,\"b\":[true,null]}")
System.print(obj["a"] == 1)                  // expect: true
System.print(obj["b"][0] == true)            // expect: true
System.print(obj["b"][1] == null)            // expect: true

System.print(Json.encode(null) == "null")    // expect: true
System.print(Json.encode(true) == "true")    // expect: true
System.print(Json.encode(12) == "12")        // expect: true
System.print(Json.encode("a\"b") == "\"a\\\"b\"")  // expect: true
System.print(Json.encode([1, 2]) == "[1,2]") // expect: true

var round = Json.parse(Json.encode({"k": "v"}))
System.print(round["k"] == "v")              // expect: true

// Escapes
System.print(Json.parse("\"a\\nb\"") == "a\nb")  // expect: true
