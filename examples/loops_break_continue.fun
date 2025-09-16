#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */
 
// break and continue examples

// 1) while loop: print odd numbers, stop after printing 7
number i = 0
while i < 10
  if i % 2 == 0
    i = i + 1
    continue
  print(i)                 // -> 1, 3, 5, 7
  if i > 5
    break
  i = i + 1

// 2) for range: skip 3, stop at 6
for i in range(0, 8)
  if i == 3
    continue
  print(i)                 // -> 0, 1, 2, 4, 5, 6
  if i == 6
    break

// 3) for-in over array: skip 3, break at 4
arr = [1, 2, 3, 4, 5]
for x in arr
  if x == 3
    continue
  print(x)                 // -> 1, 2, 4
  if x == 4
    break

/* Expected output:
1
3
5
7
0
1
2
4
5
6
1
2
4
*/
