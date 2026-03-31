// 0 is now falsy (previously 0 was truthy)

// 0 in or expression should return right side
System.print(0 || "right") // expect: right
System.print(0 || 1) // expect: 1
System.print(0 || false) // expect: false

// 0 in and expression should return 0 (short-circuit)
System.print(0 && "never") // expect: 0

// Non-zero numbers are still truthy
System.print(1 || "right") // expect: 1
System.print(-1 || "right") // expect: -1
System.print(0.1 || "right") // expect: 0.1

// 0 in if statement
if (0) {
  System.print("yes")
} else {
  System.print("no") // expect: no
}

// 0 in while loop
var count = 0
while (0) {
  count = count + 1
  if (count > 3) break
}
System.print(count) // expect: 0

// 0 in conditional expression
System.print(0 ? "yes" : "no") // expect: no
System.print(1 ? "yes" : "no") // expect: yes
