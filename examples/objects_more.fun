#!/usr/bin/env fun

// Objects as maps: nested fields, methods with explicit self

// A method to move a 2D point by dx, dy
fun move(self, dx, dy)
  self["x"] = self["x"] + dx
  self["y"] = self["y"] + dy
  return 0

// Create a point directly and inspect fields
p = { "x": 1, "y": 2, "move": move }
print(p["x"])          // -> 1
print(p["y"])          // -> 2

// Update a field directly
p["x"] = 10
print(p["x"])          // -> 10

// Call method with explicit self
move(p, 3, -2)
print(p["x"])          // -> 13
print(p["y"])          // -> 0

// Nested object example
rect = {
  "pos": { "x": 0, "y": 0 },
  "size": { "w": 5, "h": 4 }
}
print(rect["pos"]["x"])    // -> 0
print(rect["size"]["w"])   // -> 5

// Mutate nested fields
rect["pos"]["x"] = rect["pos"]["x"] + 7
rect["size"]["h"] = rect["size"]["h"] + 1
print(rect["pos"]["x"])    // -> 7
print(rect["size"]["h"])   // -> 5

/* Expected output:
1
2
10
13
0
0
5
7
5
*/
