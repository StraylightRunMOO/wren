var f0 = fn { 0 }
System.print(f0()) // expect: 0

var f1 = fn (a) { a }
System.print(f1(1)) // expect: 1

var f2 = fn (a, b) { a + b }
System.print(f2(1, 2)) // expect: 3

var f3 = fn (a, b, c) { a + b + c }
System.print(f3(1, 2, 3)) // expect: 6

var f4 = fn (a, b, c, d) { a + b + c + d }
System.print(f4(1, 2, 3, 4)) // expect: 10

var f5 = fn (a, b, c, d, e) { a + b + c + d + e }
System.print(f5(1, 2, 3, 4, 5)) // expect: 15

var f6 = fn (a, b, c, d, e, f) { a + b + c + d + e + f }
System.print(f6(1, 2, 3, 4, 5, 6)) // expect: 21

var f7 = fn (a, b, c, d, e, f, g) { a + b + c + d + e + f + g }
System.print(f7(1, 2, 3, 4, 5, 6, 7)) // expect: 28

var f8 = fn (a, b, c, d, e, f, g, h) { a + b + c + d + e + f + g + h }
System.print(f8(1, 2, 3, 4, 5, 6, 7, 8)) // expect: 36

var f9 = fn (a, b, c, d, e, f, g, h, i) { a + b + c + d + e + f + g + h + i }
System.print(f9(1, 2, 3, 4, 5, 6, 7, 8, 9)) // expect: 45

var f10 = fn (a, b, c, d, e, f, g, h, i, j) { a + b + c + d + e + f + g + h + i + j }
System.print(f10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)) // expect: 55

var f11 = fn (a, b, c, d, e, f, g, h, i, j, k) { a + b + c + d + e + f + g + h + i + j + k }
System.print(f11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)) // expect: 66

var f12 = fn (a, b, c, d, e, f, g, h, i, j, k, l) { a + b + c + d + e + f + g + h + i + j + k + l }
System.print(f12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)) // expect: 78

var f13 = fn (a, b, c, d, e, f, g, h, i, j, k, l, m) { a + b + c + d + e + f + g + h + i + j + k + l + m }
System.print(f13(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13)) // expect: 91

var f14 = fn (a, b, c, d, e, f, g, h, i, j, k, l, m, n) { a + b + c + d + e + f + g + h + i + j + k + l + m + n }
System.print(f14(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14)) // expect: 105

var f15 = fn (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) { a + b + c + d + e + f + g + h + i + j + k + l + m + n + o }
System.print(f15(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)) // expect: 120

var f16 = fn (a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) { a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p }
System.print(f16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)) // expect: 136
