#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Fibonacci: iterative and recursive

fun fib_iter(n)
  if (n <= 1) return n
  a = 0
  b = 1
  i = 2
  while (i <= n)
    t = a + b
    a = b
    b = t
    i = i + 1
  return b

fun fib_rec(n)
  if (n <= 1) return n
  return fib_rec(n - 1) + fib_rec(n - 2)

print("fib_iter(10) = " + to_string(fib_iter(10)))
print("fib_rec(10) = " + to_string(fib_rec(10)))

/* Expected output:
fib_iter(10) = 55
fib_rec(10) = 55
*/
