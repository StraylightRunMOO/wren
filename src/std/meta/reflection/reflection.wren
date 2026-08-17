// reflection - Runtime reflection and introspection
// Provides access to class metadata, methods, and instance fields

// Foreign class for low-level reflection operations
foreign class Reflection_ {
  // Get a class by name (returns ClassInfo wrapper)
  foreign static getClass(name)
  
  // Get fields of an instance (returns list of field names)
  foreign static fieldsOf(instance)
}

// Information about a class's method
class MethodInfo {
  construct new(name, arity, isGetter, isSetter, isStatic, type) {
    _name = name
    _arity = arity
    _isGetter = isGetter
    _isSetter = isSetter
    _isStatic = isStatic
    _type = type
  }

  name      { _name }
  arity     { _arity }
  isGetter  { _isGetter }
  isSetter  { _isSetter }
  isStatic  { _isStatic }
  type      { _type }
  isForeign { _type == "foreign" }
  isPrimitive { _type == "primitive" }

  toString { "MethodInfo(%(_name), arity: %(_arity), static: %(_isStatic))" }
}

// Information about a class
class ClassInfo {
  construct new(name, methods) {
    _name = name
    _methods = methods  // Map of method name -> MethodInfo
  }
  
  name { _name }
  
  // List of all method names
  methods { _methods.keys.toList }
  
  // Get arity of a method (number of args, not counting receiver)
  arity(methodName) {
    var info = _methods[methodName]
    if (info == null) Fiber.abort("Method '%(methodName)' not found.")
    return info.arity
  }
  
  // Check if class has a method (accepts base name "add" or full sig "add(_)")
  hasMethod(methodName) {
    if (_methods.containsKey(methodName)) return true
    // Check if any stored signature starts with methodName + "(" or "["
    for (key in _methods.keys) {
      var parenIdx = key.indexOf("(")
      var bracketIdx = key.indexOf("[")
      var splitIdx = -1
      if (parenIdx != -1 && bracketIdx != -1) {
        splitIdx = parenIdx < bracketIdx ? parenIdx : bracketIdx
      } else if (parenIdx != -1) {
        splitIdx = parenIdx
      } else if (bracketIdx != -1) {
        splitIdx = bracketIdx
      }
      if (splitIdx != -1 && key[0...splitIdx] == methodName) return true
    }
    return false
  }

  // Check if method is a getter (zero-arity, no parens needed)
  isGetter(methodName) {
    var info = _methods[methodName]
    if (info == null) return false
    return info.isGetter
  }

  // Check if method is a setter
  isSetter(methodName) {
    var info = _methods[methodName]
    if (info == null) return false
    return info.isSetter
  }

  // Get full MethodInfo for a method (requires full signature for overloaded methods)
  methodInfo(methodName) {
    return _methods[methodName]
  }
  
  toString { "ClassInfo(%(_name), %(_methods.count) methods)" }
}

// Main Reflection API
class reflection {
  // Get class info by name
  // Returns null if class not found
  static getClass(name) {
    if (!(name is String)) Fiber.abort("Class name must be a string.")
    
    var rawInfo = Reflection_.getClass(name)
    if (rawInfo == null) return null
    
    // rawInfo is a Map: "name" -> class name, "methods" -> Map of method info
    var className = rawInfo["name"]
    var methods = rawInfo["methods"]
    
    // Convert method info maps to MethodInfo objects
    var methodMap = {}
    for (entry in methods) {
      var mName = entry.key
      var mInfo = entry.value
      methodMap[mName] = MethodInfo.new(
        mName,
        mInfo["arity"],
        mInfo["isGetter"],
        mInfo["isSetter"],
        mInfo["isStatic"],
        mInfo["type"]
      )
    }
    
    return ClassInfo.new(className, methodMap)
  }
  
  // Get field names of an instance
  // Returns empty list for foreign classes (no runtime field info)
  static fieldsOf(instance) {
    if (instance == null) Fiber.abort("Cannot get fields of null.")
    return Reflection_.fieldsOf(instance)
  }
  
  // Check if an object has a method
  static hasMethod(obj, methodName) {
    if (obj == null) return false
    var cls = this.getClass(obj.type.name)
    if (cls == null) return false
    return cls.hasMethod(methodName)
  }
  
  // Get the ClassInfo for an object's class
  static classOf(obj) {
    if (obj == null) Fiber.abort("Cannot get class of null.")
    return this.getClass(obj.type.name)
  }
}
