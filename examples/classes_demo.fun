#!/usr/bin/env fun

// Classes demo: fields, methods (with explicit 'this'), and usage.
//
// Notes:
// - Define a class with `class Name`
// - Fields are initialized with simple assignments inside the class body.
// - Methods are written as `fun method(this, ...) { ... }` with 'this' required as the first parameter.
// - Instantiation: p = Name()
// - Method calls: load the method function from the instance into a variable, then call it with the instance as the first argument.
//   Example:
//     move_fn = p["move"]
//     move_fn(p, dx, dy)

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

// Call methods by first loading them into identifiers, then passing 'p' as first argument
to_str = p["toString"]
print("toString(p) => " + to_str(p))

move_fn = p["move"]
move_fn(p, 3, 5)
print("after move(3,5) -> " + to_str(p)) // expect Point(13, 3)

// Another instance with defaults
q = Point()
print("q initially -> " + to_str(q)) // Point(0, 0)

// Counter demo
c = Counter()
inc = c["inc"]
add = c["add"]

print("counter inc -> " + to_string(inc(c)))     // 1
print("counter inc -> " + to_string(inc(c)))     // 2
print("counter add(5) -> " + to_string(add(c, 5))) // 7
print("counter current value = " + to_string(c["value"]))
