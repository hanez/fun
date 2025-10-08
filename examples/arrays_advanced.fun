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
 * Advanced Arrays Example
 *
 * Covers more complex array features such as:
 * - Multi-dimensional arrays
 * - Array concatenation and splitting
 * - Repeating elements in arrays
 * - Array comprehensions with transformations
 * - Working with nested arrays
 */

// Advanced Arrays Demo

// Start with a basic array
arr = [1, 2, 3]
print(arr)                       // -> [1, 2, 3]

// length (also works on strings)
print(len(arr))                  // -> 3
print(len("hello"))              // -> 5

// push returns new length and mutates the array
print(push(arr, 99))             // -> 4
print(arr)                       // -> [1, 2, 3, 99]

// pop returns the removed element and mutates the array
last = pop(arr)
print(last)                      // -> 99
print(arr)                       // -> [1, 2, 3]

// set(array, index, value) sets in place and returns the value
print(set(arr, 1, 42))           // -> 42
print(arr)                       // -> [1, 42, 3]

// insert(array, index, value) inserts before index; returns new length
print(insert(arr, 1, 7))         // -> 4
print(arr)                       // -> [1, 7, 42, 3]

// remove(array, index) removes at index and returns the element
removed = remove(arr, 2)
print(removed)                   // -> 42
print(arr)                       // -> [1, 7, 3]

// slicing: a[i:j] (end is exclusive), a[i:-1] means to the end
s1 = arr[1:3]
print(s1)                        // -> [7, 3]
s2 = arr[0:-1]
print(s2)                        // -> [1, 7, 3]

// concatenation with +
tail = [10, 20]
combined = arr + tail
print(combined)                  // -> [1, 7, 3, 10, 20]

/* Expected output:
[1, 2, 3]
3
5
4
[1, 2, 3, 99]
99
[1, 2, 3]
42
[1, 42, 3]
4
[1, 7, 42, 3]
42
[1, 7, 3]
[7, 3]
[1, 7, 3]
[1, 7, 3, 10, 20]
*/
