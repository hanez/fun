#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/*
 * Classes demo: fields, methods (with explicit 'this'), and usage.
 *
 * Notes:
 * - Define a class with `class Name`
 * - Fields are initialized with simple assignments inside the class body.
 * - Methods are written as `fun method(this, ...) { ... }` with 'this' required as the first parameter.
 * - Instantiation: p = Name()
 * - Method calls: load the method function from the instance into a variable, then call it with the instance as the first argument.
 *   Example:
 *     move_fn = p["move"]
 *     move_fn(p, dx, dy)
 */

class Point
  x = 0
  y = 0
  fun move(this, dx, dy)
    this.x = this.x + dx
    this.y = this.y + dy
    return 0
  fun toString(this)
    return "Point(" + to_string(this.x) + ", " + to_string(this.y) + ")"

class Counter
  value = 0
  fun inc(this)
    this.value = this.value + 1
    return this.value
  fun add(this, n)
    this.value = this.value + n
    return this.value

// Instantiate a Point
p = Point()

// Types
print("typeof(Point) = " + typeof(Point)) // Function (factory)
print("typeof(p) = " + typeof(p))         // Map (instance)

// Read/write fields
print("p.x = " + to_string(p["x"]) + ", p.y = " + to_string(p["y"]))
p["x"] = 10
p["y"] = -2
print("after set -> p.x = " + to_string(p["x"]) + ", p.y = " + to_string(p["y"]))

// Call methods directly with dot-call sugar: p.method(args) desugars to method(p, args)
print("toString(p) => " + p.toString())

p.move(3, 5)
print("after move(3,5) -> " + p.toString()) // expect Point(13, 3)

// Another instance with defaults
q = Point()
print("q initially -> " + q.toString()) // Point(0, 0)

// Counter demo
c = Counter()
print("counter inc -> " + to_string(c.inc()))        // 1
print("counter inc -> " + to_string(c.inc()))        // 2
print("counter add(5) -> " + to_string(c.add(5)))    // 7
print("counter current value = " + to_string(c["value"]))
