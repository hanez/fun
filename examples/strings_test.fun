#!/usr/bin/env fun
// Strings test: concatenation with variables and literals, functions returning strings

print("=== strings test start ===")

string a = "Hello"
string b = "World"
string sep = ", "
string excl = "!"

// Basic concatenations
print(a + sep + b + excl)         // expect: Hello, World!
print("Hi" + " " + "there")       // expect: Hi there

// Empty strings
string empty = ""
print(empty + "x")                 // expect: x
print("x" + empty)                 // expect: x

// Function returning concatenated string
fun greet(name)
  return "Hello, " + name

print(greet("Fun"))                // expect: Hello, Fun

// Using a variable prefix with a function result
string prefix = "Hi, "
print(prefix + greet("You"))       // expect: Hi, Hello, You

// Non-empty check (string comparison)
if (a != "")
  print("a is non-empty")

print("=== strings test end ===")
