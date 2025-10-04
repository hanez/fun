#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Float type demonstration Literals: decimal and scientific notation
float a = 1.23
float b = 4.0
float c = 1e2
float d = 5E-1

// Arithmetic with promotion
float s1 = a + b       // 1.23 + 4.0 = 5.23
float s2 = c - 50      // 100.0 - 50 -> 50.0
float p  = 2 * d       // 2 * 0.5 -> 1.0
number q  = 7 / 2      // int division -> 3
float r  = 7 / 2.0     // float division -> 3.5

print("a=" + to_string(a))
print("a: " + typeof(a))
print("b=" + to_string(b))
print(typeof(b))
print("c=" + to_string(c))
print(typeof(c))
print("d=" + to_string(d))
print(typeof(d))
print("s1=" + to_string(s1))
print(typeof(s1))
print("s2=" + to_string(s2))
print(typeof(s2))
print("p=" + to_string(p))
print(typeof(p))
print("q=" + to_string(q))
print(typeof(q))
print("r=" + to_string(r))
print(typeof(r))

// to_number behavior
string s = "42.0"
number t = to_number(s)   // preserves int when exact -> 42
print("to_number(\"42.0\")=" + to_string(t))
print(typeof(t))
float u = to_number("3.14")
print("to_number(\"3.14\")=" + to_string(u))
print(typeof(u))

/* Expected output:
a=1.23
Float
b=4
Float
c=100
Float
d=0.5
Float
s1=5.2300000000000004
Float
s2=50
Float
p=1
Float
q=3
Number
r=3.5
Float
to_number("42.0")=42
Number
to_number("3.14")=3.1400000000000001
Float
*/
