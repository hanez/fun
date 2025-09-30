#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
// While loops feature test: simple loops, nested with if/else, and functions

print("=== while test start ===")

// Simple counter 0..4
number i = 0
while (i < 5)
  print(i)        // expect 0,1,2,3,4
  i = i + 1

// Nested while with if/else
number a = 3
while (a > 0)
  if (a == 2)
    print(99)     // special case when a == 2
  else
    print(a)
  a = a - 1       // expect: 3, 99, 1

// While with false condition: body should not run
while (0)
  print(123)

// Function using while
fun countdown(n)
  while (n > 0)
    print(n)
    n = n - 1
  return 0

print(countdown(3)) // expect: 3,2,1 then 0

print("=== while test end ===")
