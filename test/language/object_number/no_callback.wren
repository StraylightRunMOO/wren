// Object Numbers require a callback to be configured.
// Without it, they should produce a compilation error.
var x = #123 // expect runtime error: Object Numbers require objectNumberFn callback to be set.
