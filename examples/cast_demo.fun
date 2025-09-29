#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the ISC license.
 *
 * Added: 2025-09-29
 */

print("=== CAST demo ===")

// Number: parse decimal strings; invalid -> 0
print(cast("123", "Number"))     // -> 123
print(cast("12x", "Number"))     // -> 0
print(cast(42, "Number"))        // -> 42

// String: stringify any value
print(cast(100, "String"))       // -> "100"
print(typeof(cast(100, "String"))) // -> "String"

// Boolean: 0 -> 0, non-zero -> 1
print(cast(0, "Boolean"))        // -> 0
print(cast(42, "Boolean"))       // -> 1

// Array: arrays stay arrays; others get wrapped as single-element arrays
a = cast(42, "Array")
print(typeof(a))                  // -> "Array"
print(len(a))                     // -> 1
print(a[0])                       // -> 42

// Map: maps stay maps; others become empty maps
m = cast(42, "Map")
print(typeof(m))                  // -> "Map"
// Expect zero keys for a fresh empty map
print(len(keys(m)))               // -> 0

// Nil: always Nil
n = cast("x", "Nil")
print(typeof(n))                  // -> "Nil"

// Function: functions stay functions; others -> Nil
fun foo()
  return 7

print(typeof(cast(foo, "Function")))  // -> "Function"
print(typeof(cast(42, "Function")))   // -> "Nil"

// Chaining casts
print(cast(cast("42", "Number"), "String"))  // -> "42"

print("=== Done ===")
