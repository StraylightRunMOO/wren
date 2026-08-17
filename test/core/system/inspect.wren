var info = System.inspect("hi")
System.print(info["type"])       // expect: String
System.print(info["className"])  // expect: String
System.print(info["isClass"])    // expect: false

var cls = System.inspect(String)
System.print(cls["isClass"])     // expect: true
System.print(cls["className"])   // expect: String

var methods = cls["methods"]
System.print(methods.containsKey("+(_)"))  // expect: true
System.print(methods["+(_)"]["arity"])     // expect: 1
