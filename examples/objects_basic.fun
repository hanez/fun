#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
// Objects via maps: bracket access and method calls with explicit self

// Construct an object as a map (attach methods later)
obj = { "x": 1 }

// Property read
print(obj["x"])            // -> 1

// Property write
obj["x"] = 5
print(obj["x"])            // -> 5

// Define a method function (expects self as first parameter)
fun inc(self, d)
  // Update a field using bracket notation
  self["x"] = self["x"] + d
  return self["x"]

// Attach method and call it (explicit self)
obj["inc"] = inc
inc(obj, 3)
print(obj["x"])            // -> 8

/* Expected output:
1
5
8
*/
