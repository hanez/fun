#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */
 
// Functions test: definitions, calls, parameters, locals, returns, and if/else.

print("=== Functions test start ===")

fun add(a, b)
  return a + b

fun abs(x)
  if (x < 0)
    return -x
  else
    return x

fun sum3(a, b, c)
  // local typed declaration
  number t = a + b
  return t + c

fun id(x)
  return x

// Calls in expressions
print(add(2, 3))             // expect 5
print(add(10, -3))           // expect 7
print(abs(-7))               // expect 7
print(abs(4))                // expect 4
print(sum3(1, 2, 3))         // expect 6
print(add(add(1, 2), 3))     // expect 6
print(id(42))                // expect 42

// Call as statement (result discarded)
add(100, 23)

// Globals interacting with functions
number n = 5
print(add(n, 10))            // expect 15

print("=== Functions test end ===")
