// Example Wren script for testing the REPL

System.print("=== Wren Example Script ===")
System.print("")

// Define some variables
var pi = 3.14159
var radius = 5

// Calculate area
var area = pi * radius * radius
System.print("Circle with radius %(radius) has area %(area)")

// Define a class
class Counter {
  construct new() {
    _count = 0
  }

  increment() {
    _count = _count + 1
  }

  value { _count }
}

// Use the class
var counter = Counter.new()
counter.increment()
counter.increment()
counter.increment()

System.print("Counter value: %(counter.value)")

// Lists and functional programming
var numbers = [1, 2, 3, 4, 5]
var squared = numbers.map {|n| n * n }
var sum = numbers.reduce {|acc, n| acc + n }

System.print("Numbers: %(numbers)")
System.print("Squared: %(squared)")
System.print("Sum: %(sum)")

System.print("")
System.print("Script complete! Try typing some expressions in the REPL below:")
