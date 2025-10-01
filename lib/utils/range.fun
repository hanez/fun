/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

// Range utilities (end-exclusive)

// range(n) -> [0, 1, ..., n-1]
fun range(n)
  out = []
  number i = 0
  while i < n
    push(out, i)
    i = i + 1
  return out

// range2(start, end) -> [start, start+1, ..., end-1]
fun range2(start, end)
  out = []
  number i = start
  while i < end
    push(out, i)
    i = i + 1
  return out

// range3(start, end, step) with positive or negative step (non-zero)
fun range3(start, end, step)
  out = []
  number s = step
  if (s == 0)
    return out
  if (s > 0)
    number i = start
    while i < end
      push(out, i)
      i = i + s
  else
    number j = start
    while j > end
      push(out, j)
      j = j + s
  return out
