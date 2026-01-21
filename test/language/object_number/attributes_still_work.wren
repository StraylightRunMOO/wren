// Object Numbers should not interfere with attributes.
// Attributes use #name syntax, Object Numbers use #123 syntax.

#testattr
class Foo {
  construct new() {}

  #methodattr
  bar() {}
}

System.print("attributes work") // expect: attributes work
