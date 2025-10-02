#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

// Hex utility functions

fun hex_val(ch)
  if (ch == "0")
    return 0
  else if (ch == "1")
    return 1
  else if (ch == "2")
    return 2
  else if (ch == "3")
    return 3
  else if (ch == "4")
    return 4
  else if (ch == "5")
    return 5
  else if (ch == "6")
    return 6
  else if (ch == "7")
    return 7
  else if (ch == "8")
    return 8
  else if (ch == "9")
    return 9
  else if (ch == "a" || ch == "A")
    return 10
  else if (ch == "b" || ch == "B")
    return 11
  else if (ch == "c" || ch == "C")
    return 12
  else if (ch == "d" || ch == "D")
    return 13
  else if (ch == "e" || ch == "E")
    return 14
  else if (ch == "f" || ch == "F")
    return 15
  else 
    return 0

fun hex_to_bytes(hex)
  s = to_string(hex)
  out = []
  number i = 0
  number n = len(s)
  while i + 1 < n
    number b = hex_val(substr(s, i, 1)) * 16 + hex_val(substr(s, i + 1, 1))
    push(out, b)
    i = i + 2
  return out

fun two_hex(n)
  n = n % 256
  hexd = "0123456789abcdef"
  number hi = (n / 16) % 16
  number lo = n % 16
  c1 = substr(hexd, hi, 1)
  c2 = substr(hexd, lo, 1)
  return c1 + c2

fun bytes_to_hex(arr)
  res = ""
  number i = 0
  number N = len(arr)
  while i < N
    res = res + two_hex(arr[i])
    i = i + 1
  return res
