#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

print("=== Dynamic (untyped) ===")
x = 123
print(typeof(x))         // -> "Number"
x = "ok"
print(typeof(x))         // -> "String"

print("")
print("=== Numbers and integer subtypes ===")
number n = 42
print(typeof(n))         // -> "Number"
print(n)                 // -> 42

int8 s8 = 130            // clamps to 127
print(s8)                // -> 127
uint16 u16 = 70000       // clamps to 65535
print(u16)               // -> 65535
byte b = 0xFF            // 255
print(b)                 // -> 255
boolean flag = 2         // clamps to 1
print(flag)              // -> 1

print("")
print("=== Strings ===")
string s = "hello"
print(typeof(s))         // -> "String"
print(s)                 // -> "hello"

print("")
print("=== Nil ===")
nil nn = nil
print(typeof(nn))        // -> "Nil"
print(nn)                // -> nil

print("")
print("=== Arrays ===")
arr = [1, 2, 3]
print(typeof(arr))       // -> "Array"
print(len(arr))          // -> 3
print(arr[1])            // -> 2
print(arr[0:2])          // -> [1, 2]

print("")
print("=== Maps ===")
m = { "k": "v", "n": 1 }
print(typeof(m))         // -> "Map"
print(m["k"])            // -> "v"

print("")
print("=== Functions ===")
fun add(a, b)
  return a + b
print(typeof(add))       // -> "Function"
print(add(5, 7))         // -> 12

print("")
print("=== Classes and Class-typed variables ===")
class Person(number age, string name)
  // methods must declare 'this' as first parameter
  fun toString(this)
    // typeof(instance) will call this and return it
    return "Class"

Class p = Person(33, "Jo")
print(typeof(p))         // -> "Class"
// p = "nope"            // Uncomment to see TypeError: expected Class

print("")
print("=== Done ===")
