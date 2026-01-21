^title Fn Class

A first class function&mdash;an object that wraps an executable chunk of code.
[Here][functions] is a friendly introduction.

[functions]: ../../functions.html

## Methods

### **arity**

The number of arguments the function requires.

<pre class="snippet">
System.print(fn {}.arity)             //> 0
System.print(fn (a, b, c) { a }.arity) //> 3
</pre>

### **call**(args...)

Invokes the function with the given arguments.

<pre class="snippet">
var fn = fn (arg) {
  System.print(arg)     //> Hello world
}

fn.call("Hello world")
</pre>

When a function is followed immediately by a parenthesized argument list, that is syntax sugar for calling `call`. These two forms are exactly equivalent:

<pre class="snippet">
var fn = fn (a, b) { a + b }
System.print(fn.call(1, 2)) //> 3
System.print(fn(1, 2))      //> 3
</pre>

It is a runtime error if the number of arguments given is less than the arity
of the function. If more arguments are given than the function's arity they are
ignored.
