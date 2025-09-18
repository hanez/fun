#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/*
 * Conditional Logic Testing Suite
 *
 * Tests various aspects of conditional execution including:
 * - Basic if/else structures
 * - Nested conditional statements
 * - Ternary operator usage (`? :`)
 * - Comparison conditionals with different operators
 */

// Test else / else if chains and nested if-blocks (two-space indentation)

print("=== if/else-if/else test ===")

number n = 42

if (n < 0)
  print("neg")
else if (n == 0)
  print("zero")
else if (n < 10)
  print("small")
else
  print("big")        // expect: big

// Nested if inside a true branch
if (n >= 10)
  print(n)            // expect: 42
  if (n == 42)
    print("answer")   // expect: answer

print("=== if/else-if/else done ===")
