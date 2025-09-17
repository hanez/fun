#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// Demonstration of enhanced typeof behavior:
//
// - typeof(<identifier>) for declared integer types returns the subtype:
//     Sint8, Sint16, Sint32, Sint64, Uint8, Uint16, Uint32, Uint64
// - typeof(<expression>) or non-integer identifiers returns runtime categories:
//     Number, String, Array, Map, Function, Nil

// Integer subtypes (declared)
sint8 si8 = -1
sint16 si16 = -32768
sint32 si32 = -2147483648
sint64 si64 = -9223372036854775808

uint8 ui8  = 255
uint16 ui16 = 65535
uint32 ui32 = 4294967295
uint64 ui64 = 18446744073709551615

// 'number' maps to Sint64 behavior by design
number n = 42

// Other runtime types
string s = "hello"
arr = [1, 2, 3]
m = { "k": 1 }

fun f()
  return nil

nil x = f()

// typeof on identifiers (for integers => declared subtype)
print("typeof(si8)  = " + typeof(si8))   // Sint8
print("typeof(si16) = " + typeof(si16))  // Sint16
print("typeof(si32) = " + typeof(si32))  // Sint32
print("typeof(si64) = " + typeof(si64))  // Sint64

print("typeof(ui8)  = " + typeof(ui8))   // Uint8
print("typeof(ui16) = " + typeof(ui16))  // Uint16
print("typeof(ui32) = " + typeof(ui32))  // Uint32
print("typeof(ui64) = " + typeof(ui64))  // Uint64

print("typeof(n)    = " + typeof(n))     // Sint64

// typeof on identifiers for non-integers -> runtime categories
print("typeof(s)    = " + typeof(s))     // String
print("typeof(arr)  = " + typeof(arr))   // Array
print("typeof(m)    = " + typeof(m))     // Map
print("typeof(f)    = " + typeof(f))     // Function
print("typeof(x)    = " + typeof(x))     // Nil

// typeof on expressions -> runtime category (Number), not the declared subtype
print("typeof(ui8 + 1) = " + typeof(ui8 + 1))  // Number
print("typeof(si16 - 2) = " + typeof(si16 - 2))// Number

// Demonstrate that subtype remains on identifier after operations (assignment clamps)
ui8 = ui8 + 10
print("ui8 value = " + to_string(ui8) + ", typeof(ui8) = " + typeof(ui8)) // Uint8

si8 = si8 - 5
print("si8 value = " + to_string(si8) + ", typeof(si8) = " + typeof(si8)) // Sint8
