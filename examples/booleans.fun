#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// Boolean datatype usage examples (first-class booleans)
// Run like:
//   FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/booleans.fun

// Literals
boolean t = true
print("typeof(t) => " + typeof(t))
boolean f = false
print("typeof(f) => " + typeof(f))

print("t literal => " + to_string(t))          // "true"
print("f literal => " + to_string(f))          // "false"

// Logical operators (short-circuit semantics)
print("true && true  => " + to_string(true && true))
print("true && false => " + to_string(true && false))
print("false || true => " + to_string(false || true))
print("false || false => " + to_string(false || false))
print("!true => " + to_string(!true))
print("!false => " + to_string(!false))

// Equality and inequality
print("true == true => " + to_string(true == true))
print("true != false => " + to_string(true != false))

// Interoperability with 0/1 (still supported)
print("true == 1 => " + to_string(true == 1))
print("false == 0 => " + to_string(false == 0))
print("1 == true => " + to_string(1 == true))
print("0 != true => " + to_string(0 != true))

// Use in conditionals
if t
  print("if t: branch taken")
else
  print("if t: branch NOT taken")

if f
  print("if f: branch taken (unexpected)")
else
  print("if f: else branch taken")

/* Expected output:
typeof(t) => Boolean
typeof(f) => Boolean
t literal => true
f literal => false
true && true  => true
true && false => false
false || true => 1
false || false => 0
!true => false
!false => true
true == true => true
true != false => true
true == 1 => true
false == 0 => true
1 == true => true
0 != true => true
if t: branch taken
if f: else branch taken
*/

