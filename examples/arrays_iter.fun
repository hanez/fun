#!/usr/bin/env fun

// for-in over an array literal
for x in [1, 2, 3]
  print(x)                 // prints: 1, then 2, then 3

// for-in over a named array
arr = [10, 20, 30]
for x in arr
  print(x)                 // prints: 10, then 20, then 30

// nested arrays: iterate rows, then elements
matrix = [[1, 2], [3, 4]]
for row in matrix
  print(row)               // prints: [1, 2] then [3, 4]
  for v in row
    print(v)               // prints: 1, 2, 3, 4

// iterate over an array expression (concatenation)
for v in arr + [40, 50]
  print(v)                 // prints: 10, 20, 30, 40, 50

// iterate over a slice (end = -1 means till the end)
for v in arr[1:-1]
  print(v)                 // prints: 20, 30

// iterate over a function result
fun make()
  return [7, 9, 11]

for v in make()
  print(v)                 // prints: 7, 9, 11

/* Expected output:
1
2
3
10
20
30
[1, 2]
1
2
[3, 4]
3
4
10
20
30
40
50
20
30
7
9
11
*/
