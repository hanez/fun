#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

print("=== Typeof test start ===")
 
 // Array
a = [1, 2, 3]
print(typeof(a))
 
fun f()
  print("A function!")

// Function
print(typeof(f))
f()

// Map
m = { "x": 1, "y": 2, "move": move }
print(typeof(m))

// Nil
nil x = nil
print(typeof(x))

// Number
number n = 10
print(typeof(n)) 

string s = "Have fun!"
print(typeof(s)) 

print("=== Typeof test end ===")

/* Expected output:
Array
Function
A function!
Map
Nil
Number
String
*/
