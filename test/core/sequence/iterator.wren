// Sequence.iterator() returns a Generator backed by the host runtime.
var g = [1, 2, 3].iterator()
System.print(g.next())  // expect: 1
System.print(g.next())  // expect: 2
System.print(g.next())  // expect: 3
System.print(g.next())  // expect: null

var sum = 0
for (n in 1..4) sum = sum + n
System.print(sum)  // expect: 10
