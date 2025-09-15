#!/usr/bin/env fun

// Arrays basics
arr = [1, 2, 3]
print(arr)                 // -> [1, 2, 3]
print(arr[0] + arr[1])     // -> 3

// mutate last element
arr[2] = arr[2] + 10
print(arr)                 // -> [1, 2, 13]

// Sum elements with while (fixed length)
number i = 0
number sum = 0
number len = 3
while i < len
  sum = sum + arr[i]
  i = i + 1
print(sum)                 // -> 16

// Fill with squares using for in range
arr = [0, 0, 0, 0, 0]
for i in range(0, 5)
  arr[i] = i * i
print(arr)                 // -> [0, 1, 4, 9, 16]

// Nested arrays (matrix)
matrix = [[1, 2], [3, 4], [5, 6]]
print(matrix)              // -> [[1, 2], [3, 4], [5, 6]]
print(matrix[1][0])        // -> 3

// Function that mutates an array in place (first 3 elements)
fun scale3(a, s)
  number i = 0
  while i < 3
    a[i] = a[i] * s
    i = i + 1

arr = [2, 4, 6]
scale3(arr, 5)
print(arr)                 // -> [10, 20, 30]

// Function that returns an array
fun pair(x, y)
  return [x, y]

p = pair(7, 9)
print(p)                   // -> [7, 9]
print(p[1])                // -> 9

/* Expected output:
[1, 2, 3]
3
[1, 2, 13]
16
[0, 1, 4, 9, 16]
[[1, 2], [3, 4], [5, 6]]
3
[10, 20, 30]
[7, 9]
9
*/
