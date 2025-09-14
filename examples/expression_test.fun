#!/usr/bin/env fun

// Feature test for current Fun parser/VM.

print("=== Feature Test Start ===")

// Typed declarations
number a = 2
number b = 3
number n = 10
string s = "Hello, world!"
boolean flag = true

// Arithmetic and precedence
number sum = a + b * 5 - 1
print(sum)           // expect 2 + 3 * 5 - 1 = 16

// Unary operators
number neg = -n
print(neg)           // expect -10

number modv = n % 3
print(modv)          // expect 1

// Booleans and logical ops
print(!false)        // expect 1 (true)
print(true && false) // expect 0 (false)
print(true || false) // expect 1 (true)

// Comparisons and strings
print(n > 5)         // expect 1
print(s != "")       // expect 1

// Indentation-based nested if blocks (two spaces)
if (n >= 10)
  print(n)           // expect 10
  if (a + b == 5)
    print(42)        // expect 42

print("=== Feature Test End ===")
