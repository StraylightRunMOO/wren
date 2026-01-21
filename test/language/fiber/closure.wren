var fiber
var closure

{
  var a = "before"
  fiber = Fiber.new {
    System.print(a)
    Fiber.yield()
    a = "after"
    Fiber.yield()
    System.print(a)
    a = "final"
  }

  closure = fn {
    System.print(a)
  }
}

fiber.call()   // expect: before
closure() // expect: before
fiber.call()
closure() // expect: after
fiber.call()   // expect: after
closure() // expect: final
