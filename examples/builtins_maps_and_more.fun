#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */
 
// Maps, map/filter/reduce, labeled break/continue (depth), and file I/O

// Map literal and indexing
m = { "a": 1, "b": 2 }
print(m)                         // -> {"a": 1, "b": 2} (order may vary)
print(m["a"])                    // -> 1
m["c"] = 5
print(has(m, "c"))               // -> 1
print(keys(m))                   // -> ["a", "b", "c"] (order may vary)
print(values(m))                 // -> [1, 2, 5]       (order may vary)

// map/filter/reduce
nums = [1, 2, 3, 4]

fun double(x)
  return x + x

fun is_odd(x)
  return x % 2 == 1

fun sum2(a, b)
  return a + b

print(map(nums, double))         // -> [2, 4, 6, 8]
print(filter(nums, is_odd))      // -> [1, 3]
print(reduce(nums, 0, sum2))     // -> 10

// labeled break/continue by depth using nested loops
for i in range(0, 3)
  for j in range(0, 5)
    if j == 1
      continue 1                 // skip to next j
    if i == 1 && j == 2
      break 2                    // break outer and inner
    print(i * 10 + j)

// file I/O (writes then reads)
path = "tmp_io_example.txt"
ok = write_file(path, "hello-io")
print(ok)                        // -> 1
content = read_file(path)
print(content)                   // -> hello-io

/* Expected output:
{"a": 1, "b": 2}
1
1
["a", "b", "c"]
[1, 2, 5]
[2, 4, 6, 8]
[1, 3]
10
0
2
3
1
hello-io
*/
