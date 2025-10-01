#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

include <strings.fun>

fun foo()
  print("Have fun!")
  print("Having fun... forever.")

print("Typeof foo(): " + typeof(foo))

print("Yay, the playground for having fun... ;)")

print(string_to_bytes_ascii("Have Fun!"))

number n = 23
// Every type MUST be lowercase. Sint* must be sint*.
print(n)

// This MUST fail because n is of type number and can not become a string or function. Setting it to 0 is not an option.
n = "FooBar"
print(n)
print("Typeof n: " + typeof(n))

n = 100
print(n)
print("Typeof n: " + typeof(n))

fun n(num)
  print("n(" + to_string(num) + ")")
n(42)
// Typeof n MUST be of type Function here... Not Sint64.
print("Typeof n: " + typeof(n))

n = 2342
print(n)
print("Typeof n: " + typeof(n))

x = 42
print (x)

x = "BarFoo"
print(x)

// Arrays must be declared with an "array" identifier if not beeing dynamicly typed. We need the "array" type."
a = [23, 42]
print(a)
print(a[0])

// Why this is possible? Setting n to a string, sets n to 0. Setting an array to 1 works...? This must fail when an a is of type "array", not in this case!
a = 1
print(a)

string s = 'Have\n"fun!"'
  print(s)

print("\'" + " Fun")
print('\'' + " \'Fun\'")
print('\'' + " \"Fun\"")
print('\'' + " \n\"Fun\"")
print('\'' + " \n\t\"Fun\"")

print("Running foo() 2 times...")
foo()
// We need arguments to functions...
foo()
