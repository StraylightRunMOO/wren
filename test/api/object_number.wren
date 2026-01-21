// Test basic Object Numbers
// For this test, Object Numbers simply return their numeric value
var obj1 = #123
var obj2 = #456
var obj3 = #0

System.print("obj1 = %(obj1)")  // expect: obj1 = 123
System.print("obj2 = %(obj2)")  // expect: obj2 = 456
System.print("obj3 = %(obj3)")  // expect: obj3 = 0

// Test Object Numbers in expressions
var listResult = [#1, #2, #3]
System.print("list length = %(listResult.count)")  // expect: list length = 3
System.print("listResult[0] = %(listResult[0])")  // expect: listResult[0] = 1
System.print("listResult[1] = %(listResult[1])")  // expect: listResult[1] = 2
System.print("listResult[2] = %(listResult[2])")  // expect: listResult[2] = 3

// Test large values
var large = #999
System.print("large = %(large)")  // expect: large = 999

// Test in arithmetic
var sum = #10 + #20
System.print("sum = %(sum)")  // expect: sum = 30
