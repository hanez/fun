#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/*
 * # Extended Built-ins Examples
 *
 * Demonstrates advanced usage of built-in types including:
 * - Customizing default behaviors with `default` keyword
 * - Working with large integer values (beyond standard limits)
 * - Implementing type-specific extensions
 *- Handling edge cases in conversions and operations
*/

// Built-ins: strings, array utils, iteration helpers, and math

// Strings
print(join(split("a,b,c", ","), "-"))    // -> a-b-c
print(substr("abcdef", 2, 3))            // -> cde
print(find("hello world", "wor"))        // -> 6

// Array utils
arr = [1, 2, 3, 2]
print(contains(arr, 2))                  // -> 1
print(indexOf(arr, 3))                   // -> 2
clear(arr)
print(len(arr))                          // -> 0

// Iteration helpers
nums = [10, 20, 30]
print(enumerate(nums))                   // -> [[0, 10], [1, 20], [2, 30]]
more = [100, 200, 300, 400]
print(zip(nums, more))                   // -> [[10, 100], [20, 200], [30, 300]]

// Math
print(min(5, 9))                         // -> 5
print(max(5, 9))                         // -> 9
print(clamp(15, 0, 10))                  // -> 10
print(abs(-7))                           // -> 7
print(pow(2, 8))                         // -> 256
random_seed(123)                              // seed RNG
print(random_int(0, 3))                   // -> 0..2 (deterministic for seed 123)
print(random_int(5, 6))                   // -> 5

/* Expected output:
a-b-c
cde
6
1
2
0
[[0, 10], [1, 20], [2, 30]]
[[10, 100], [20, 200], [30, 300]]
5
9
10
7
256
0
5
*/
