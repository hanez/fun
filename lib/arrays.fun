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

// Array utilities

// Return a shallow slice of arr starting at 'start' with 'count' elements (clamped)
fun array_slice(arr, start, count)
  number n = len(arr)
  number s = start
  if (s < 0)
    s = 0
  if (s > n)
    s = n
  number c = count
  if (c < 0)
    c = 0
  if (s + c > n)
    c = n - s
  out = []
  number i = 0
  while i < c
    push(out, arr[s + i])
    i = i + 1
  return out

// Return a new array with elements in reverse order
fun array_reverse(arr)
  out = []
  number i = len(arr) - 1
  while i >= 0
    push(out, arr[i])
    i = i - 1
  return out

// Concatenate two arrays and return a new array
fun array_concat(a, b)
  out = []
  number i = 0
  while i < len(a)
    push(out, a[i])
    i = i + 1
  number j = 0
  while j < len(b)
    push(out, b[j])
    j = j + 1
  return out

// Return the index of the first occurrence of value in arr, or -1
fun array_index_of(arr, value)
  number i = 0
  number n = len(arr)
  while i < n
    if (arr[i] == value)
      return i
    i = i + 1
  return -1

// Return 1 if arr contains value, else 0
fun array_contains(arr, value)
  return array_index_of(arr, value) >= 0

// Return a new array with only the first occurrence of each element (stable)
fun array_unique(arr)
  out = []
  number i = 0
  number n = len(arr)
  while i < n
    v = arr[i]
    if (array_index_of(out, v) < 0)
      push(out, v)
    i = i + 1
  return out

// Flatten one level: [[1,2],[3],[4,5]] -> [1,2,3,4,5]
fun array_flatten1(arr)
  out = []
  number i = 0
  number n = len(arr)
  while i < n
    item = arr[i]
    // Treat nested arrays as flattenable
    if (typeof(item) == "Array")
      number j = 0
      number m = len(item)
      while j < m
        push(out, item[j])
        j = j + 1
    else
      push(out, item)
    i = i + 1
  return out

// Push value to arr only if not present (mutates arr), returns arr
fun array_push_unique_mut(arr, value)
  if (!array_contains(arr, value))
    push(arr, value)
  return arr

// Return a new array that contains all elements of arr and value appended only if not present
fun array_push_unique(arr, value)
  out = []
  number i = 0
  number n = len(arr)
  while i < n
    push(out, arr[i])
    i = i + 1
  if (!array_contains(out, value))
    push(out, value)
  return out
