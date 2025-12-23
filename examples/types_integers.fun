#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Integer (number) and boolean usage examples

// Basic typed integers
number a = 42
number b = -7
number zero = 0

print("a = " + to_string(a) + " :: " + typeof(a))       // "Number"
print("b = " + to_string(b) + " :: " + typeof(b))       // "Number"
print("zero = " + to_string(zero) + " :: " + typeof(zero))

// Arithmetic with integers
number sum = a + b
number diff = a - b
number prod = a * 3
number quot = a / 2
number rem = a % 5

print("sum (a + b) = " + to_string(sum))
print("diff (a - b) = " + to_string(diff))
print("prod (a * 3) = " + to_string(prod))
print("quot (a / 2) = " + to_string(quot))
print("rem (a % 5) = " + to_string(rem))

// Comparisons (results are numbers 1/0 in Fun)
print("a > b = " + to_string(a > b))
print("a >= b = " + to_string(a >= b))
print("a < b = " + to_string(a < b))
print("a <= b = " + to_string(a <= b))
print("a == b = " + to_string(a == b))
print("a != b = " + to_string(a != b))

// Booleans are numeric 1/0; typeof returns "Number"
boolean t = true
boolean f = false
print("t = " + to_string(t) + " :: " + typeof(t))
print("f = " + to_string(f) + " :: " + typeof(f))

// Short-circuit logic with numeric truthiness
print("t && (a > 0) = " + to_string(t && (a > 0)))
print("f || (a < 0) = " + to_string(f || (a < 0)))

// Using integers in a for-range loop: sum 1..5
fun sum_range(lo, hi)
  number acc = 0
  for i in range(lo, hi + 1)
    acc = acc + i
  return acc

print("sum 1..5 = " + to_string(sum_range(1, 5)))

// Arrays of integers
nums = [1, 2, 3, 4, 5]
print("nums length = " + to_string(len(nums)))
print("nums[2] = " + to_string(nums[2]))

// Map with numeric fields
pt = { "x": 10, "y": -3 }
print("pt['x'] = " + to_string(pt["x"]) + " :: " + typeof(pt["x"]))
print("pt['y'] = " + to_string(pt["y"]) + " :: " + typeof(pt["y"]))

// Conversions
print("to_string(123) => " + to_string(123))
print("to_number(\"456\") => " + to_string(to_number("456")))

/* Expected output:
a = 42 :: Sint64
b = -7 :: Sint64
zero = 0 :: Sint64
sum (a + b) = 35
diff (a - b) = 49
prod (a * 3) = 126
quot (a / 2) = 21
rem (a % 5) = 2
a > b = 1
a >= b = 1
a < b = 0
a <= b = 0
a == b = false
a != b = true
t = true :: Boolean
f = false :: Boolean
t && (a > 0) = true
f || (a < 0) = 0
sum 1..5 = 15
nums length = 5
nums[2] = 3
pt['x'] = 10 :: Number
pt['y'] = -3 :: Number
to_string(123) => 123
to_number("456") => 456
*/

