/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

// Math helpers

fun abs(x)
  if (x < 0)
    return -x
  return x

fun clamp(x, lo, hi)
  v = x
  if (v < lo) v = lo
  if (v > hi) v = hi
  return v

fun gcd(a, b)
  number x = abs(a)
  number y = abs(b)
  while y != 0
    t = x % y
    x = y
    y = t
  return x

fun lcm(a, b)
  if (a == 0) || (b == 0)
    return 0
  return abs(a * b) / gcd(a, b)

// integer exponent (exp >= 0)
fun powi(base, exp)
  number e = exp
  if (e < 0)
    // simple fallback for negative: 0 for now (no floats)
    return 0
  number result = 1
  number b = base
  while e > 0
    if (e % 2 == 1)
      result = result * b
    b = b * b
    e = e / 2
  return result

// Min/Max over three numbers
fun min3(a, b, c)
  m = a
  if (b < m) m = b
  if (c < m) m = c
  return m

fun max3(a, b, c)
  m = a
  if (b > m) m = b
  if (c > m) m = c
  return m

// Array min/max (for non-empty arrays)
fun array_min(arr)
  number n = len(arr)
  if (n == 0)
    return 0
  m = arr[0]
  number i = 1
  while i < n
    if (arr[i] < m)
      m = arr[i]
    i = i + 1
  return m

fun array_max(arr)
  number n = len(arr)
  if (n == 0)
    return 0
  m = arr[0]
  number i = 1
  while i < n
    if (arr[i] > m)
      m = arr[i]
    i = i + 1
  return m
