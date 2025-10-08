#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
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
