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

// Selection sort + linear/binary search

fun sel_sort(a)
  n = len(a)
  i = 0
  while (i < n)
    m = i
    j = i + 1
    while (j < n)
      if (a[j] < a[m]) m = j
      j = j + 1
    tmp = a[i]
    a[i] = a[m]
    a[m] = tmp
    i = i + 1
  return a

fun lin_find(a, x)
  i = 0
  while (i < len(a))
    if (a[i] == x) return i
    i = i + 1
  return -1

fun bin_find(a, x)
  lo = 0
  hi = len(a) - 1
  while (lo <= hi)
    mid = (lo + hi) / 2
    v = a[mid]
    if (v == x) return mid
    if (v < x)
      lo = mid + 1
    else
      hi = mid - 1
  return -1

arr = [5,1,4,2,3]
print("sorted: " + to_string(sel_sort(arr)))
print("lin_find 4: " + to_string(lin_find(arr, 4)))
print("bin_find 4: " + to_string(bin_find(arr, 4)))

/* Expected output:
sorted: [array n=5]
lin_find 4: 2
bin_find 4: 2
*/
