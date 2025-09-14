#!/usr/bin/env fun

// Test else / else if chains and nested if-blocks (two-space indentation)

print("=== if/else-if/else test ===")

number n = 42

if (n < 0)
  print("neg")
else if (n == 0)
  print("zero")
else if (n < 10)
  print("small")
else
  print("big")        // expect: big

// Nested if inside a true branch
if (n >= 10)
  print(n)            // expect: 42
  if (n == 42)
    print("answer")   // expect: answer

print("=== if/else-if/else done ===")
