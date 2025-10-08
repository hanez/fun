#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
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

class Point(number x, number y) // I don't like Uppercase or CamelCase, and core libs of Fun will not contain this, but "_" and all lowercase. It's a showup here... ;)
  x = 0
  y = 0
  /* private helper (callable only as this._shift inside class) */
  fun _shift(this, dx, dy)
    this.x = this.x + dx
    this.y = this.y + dy
    return 0
  fun move(this, dx, dy)
    /* delegate to private helper */
    this._shift(dx, dy)
    print("typeof(this._shift) = " + typeof(this._shift))
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

// Instantiate a Point with constructor parameters
p = Point(10, -2)

// Types
print("typeof(Point) = " + typeof(Point)) // Class (factory)
print("typeof(p) = " + typeof(p))         // Point(10, -2) (instance)

// Read/write fields
print("p.x = " + to_string(p["x"]) + ", p.y = " + to_string(p["y"]))

// Call methods directly with dot-call sugar: p.method(args) desugars to method(p, args)
print("toString(p) => " + p.toString())

p.move(3, 5)
print("after move(3,5) -> " + p.toString()) // expect Point(13, 3)

/* External private call should fail with AccessError and halt */
// This fails because of calling a private method, prefixed with "_" in the class definition. Commented out to make the
// script run without any errors.
//p._shift(1, 1)

// Another instance (explicit constructor args)
q = Point(0, 0)
print("q initially -> " + q.toString()) // Point(0, 0)

// Counter demo
c = Counter()
print("counter inc -> " + to_string(c.inc()))        // 1
print("counter inc -> " + to_string(c.inc()))        // 2
print("counter add(5) -> " + to_string(c.add(5)))    // 7
print("counter current value = " + to_string(c["value"]))

class HaveFun()
  fun have_fun(this)
    print("Have Fun!")

HaveFun().have_fun()

/* Expected output:
typeof(Point) = Class
typeof(p) = Point(10, -2)
p.x = 10, p.y = -2
toString(p) => Point(10, -2)
typeof(this._shift) = Function
after move(3,5) -> Point(13, 3)
q initially -> Point(0, 0)
counter inc -> 1
counter inc -> 2
counter add(5) -> 7
counter current value = 7
Have Fun!
*/
