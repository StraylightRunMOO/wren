var f = fn {
  System.print(Global)
}

var Global = "global"

f() // expect: global