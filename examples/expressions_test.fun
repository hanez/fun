#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/*
 * Expression Testing Suite
 *
 * Contains comprehensive tests for:
 * - Operator precedence rules
 * - Arithmetic expression evaluation
 * - Logical expressions (AND, OR)
 * - Conditional expressions (`if`, `else`)
 * - Expression short-circuiting behavior
 */

// Expressions test: arithmetic, precedence, comparisons, logical ops, unary, parentheses.

print("=== Expressions test start ===")

// Typed declarations
number a = 2
number b = 3
number n = 10
string s = "Have fun!"
boolean flag = true

// Arithmetic and precedence
number sum = a + b * 5 - 1
print(sum)           // expect 2 + 3*5 - 1 = 16

number calc = (a + b) * (b - a + 1)
print(calc)          // expect (2+3)*(3-2+1) = 5*2 = 10

// Unary operators
number neg = -n
print(neg)           // expect -10

number modv = n % 3
print(modv)          // expect 1

// Booleans and logical ops
print(!false)        // expect 1 (true)
print(true && false) // expect 0 (false)
print(true || false) // expect 1 (true)

// Comparisons and strings
print(n > 5)         // expect 1
print(s != "")       // expect 1

// Mixed logical/relational
print(a < b && n >= 10)          // expect 1
print((a + b) == 5 && flag)      // expect 1

print("=== Expressions test end ===")

/* Expected output:
=== Expressions test start ===
16
10
-10
1
true
false
true
1
true
true
true
=== Expressions test end ===
*/

