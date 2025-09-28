#!/usr/bin/env fun

/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// Type Safety demonstration in Fun

print("== Dynamic variable (untyped) ==")
x = 100
print(typeof(x))   // -> "Number"
print(x)          // -> 100

x = "hello"
print(typeof(x))   // -> "String"
print(x)          // -> "hello"

print("")
print("== Typed variables remain type-stable ==")

// Strings must always be strings
string s = "hello"
print(typeof(s))   // -> "String"
print(s)           // -> "hello"
// s = 42          // Uncomment to see runtime TypeError: expected String

// Booleans are Numbers constrained to 0 or 1 (clamped)
boolean flag = 0
print(typeof(flag)) // -> "Number"
print(flag)         // -> 0

flag = 2            // will be clamped to 1
print(flag)         // -> 1
// flag = "no"      // Uncomment to see runtime TypeError: expected Number for boolean

// number is a signed 64-bit integer and must remain Number
number n = 123
print(typeof(n))    // -> "Number"
n = 456
print(n)            // -> 456
// n = "oops"       // Uncomment to see runtime TypeError: expected Number

print("")
print("== Integer width clamping ==")

// Signed 8-bit: range [-128..127]
int8 si = 130       // clamps to 127
print(si)           // -> 127
si = -200           // clamps to -128
print(si)           // -> -128

// Unsigned 8-bit: range [0..255]
uint8 u = 255
print(u)            // -> 255
u = 300             // clamps to 255
print(u)            // -> 255
// u = "bad"        // Uncomment to see runtime TypeError: expected Number

print("")
print("== Nil-typed variables must stay Nil ==")

nil nn = nil
print(typeof(nn))   // -> "Nil"
// nn = 0           // Uncomment to see runtime TypeError: expected Nil

print("")
print("Done. Uncomment lines above to see type errors in action.")
