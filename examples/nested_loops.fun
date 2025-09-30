#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
// Nested loops: break and continue behavior

// 1) Nested for range: inner continue on j==2, inner break on j==4, outer break on i==3
for i in range(0, 5)
  for j in range(0, 5)
    if j == 2
      continue            // skip j==2
    if j == 4
      break               // stop inner loop at j==4
    print(i * 10 + j)     // e.g., i=0 -> 0, 1, 3
  if i == 3
    break                 // stop outer loop when i==3

// 2) Nested for-in over arrays: skip even values, break inner when hitting 7
rows = [[1, 2, 3], [4, 5], [6, 7, 8, 9]]    
for row in rows
  for v in row
    if v % 2 == 0
      continue            // skip even numbers
    print(v)              // prints odd numbers in each row
    if v == 7
      break               // stop inner loop when reaching 7

// 3) Nested while loops: inner continue/break and regular increments
number i = 0
while i < 3
  number k = 0
  while k < 5
    if k == 1
      k = k + 1
      continue            // skip k==1
    print(i * 100 + k)
    if k == 3
      break               // exit inner loop at k==3
    k = k + 1
  i = i + 1

/* Expected output:
0
1
3
10
11
13
20
21
23
30
31
33
1
3
5
7
0
2
3
100
102
103
200
202
203
*/
