#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */
 
// for/range test: globals, nested loops, and a function using range

print("=== for/range test start ===")

// Basic: 0..4
for i in range(0, 5)
  print(i)            // expect: 0,1,2,3,4

// Using variables for bounds
number start = 2
number end = 6
for j in range(start, end)
  print(j)            // expect: 2,3,4,5

// Function that sums a half-open range [a, b)
fun sum_range(a, b)
  number acc = 0
  for k in range(a, b)
    acc = acc + k
  return acc

print(sum_range(1, 4)) // expect: 6 (1+2+3)

// Nested ranges
for x in range(1, 3)
  for y in range(1, 3)
    print(x + y)      // expect: 2,3,3,4

print("=== for/range test end ===")
// for/range test: globals, nested loops, and a function using range

print("=== for/range test start ===")

// Basic: 0..4
for i in range(0, 5)
  print(i)            // expect: 0,1,2,3,4

// Using variables for bounds
number start = 2
number end = 6
for j in range(start, end)
  print(j)            // expect: 2,3,4,5

// Function that sums a half-open range [a, b)
fun sum_range(a, b)
  number acc = 0
  for k in range(a, b)
    acc = acc + k
  return acc

print(sum_range(1, 4)) // expect: 6 (1+2+3)

// Nested ranges
for x in range(1, 3)
  for y in range(1, 3)
    print(x + y)      // expect: 2,3,3,4

print("=== for/range test end ===")

/*
=== for/range test start ===
0
1
2
3
4
2
3
4
5
6
2
3
3
4
=== for/range test end ===
=== for/range test start ===
0
1
2
3
4
2
3
4
5
6
2
3
3
4
=== for/range test end ===
*/
