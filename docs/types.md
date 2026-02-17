# Core datatypes in Fun: arrays, maps, strings, numbers, booleans and nil

This guide shows how to create and use the most common Fun datatypes with practical examples. It focuses on arrays and maps, and also recaps numbers, floats, strings, booleans and nil so examples are self‑contained.

See also:

- [arrays.md](./arrays.md) — Deeper dive into array creation, indexing/slicing, iteration, and helpers.
- [strings.md](./strings.md) — Practical guide to string literals, concatenation, substr/find, split, and conversions.
- [maps.md](./maps.md) — Detailed guide to map construction, lookup/update, merging, and patterns.
- [numbers.md](./numbers.md) — Numbers vs floats, arithmetic, conversions, clamping, bitwise ops, and common patterns.

## Quick overview

- number: signed integer (use uclamp/sclamp for width handling)
- float: 64‑bit floating point
- string: immutable text/bytes
- array: ordered, zero‑indexed sequence
- map: associative dictionary (usually string keys)
- boolean: 1 (true) or 0 (false)
- nil: absence of a value

Helpers used throughout:
- typeof(x) → string type name
- to_string(x), to_number(x), cast(x, typeName)
- len(x) works for strings and arrays

## Arrays

Create arrays with square brackets and comma‑separated elements.

```
// creation
a = [1, 2, 3]
b = ["alpha", "beta"]
c = []  // empty array

print(typeof(a))          // "array"
print(len(a))             // 3

// indexing (0-based)
print(a[0])               // 1
print(a[2])               // 3

// set by index
a[1] = 42
print(a)                  // [1, 42, 3]

// push/pop (append and pop-last)
push(a, 7)                // returns new length (4)
v = apop(a)               // removes and returns last element (7)

// insert/remove at index
insert(a, 1, 99)          // a becomes [1, 99, 42, 3]
remove(a, 2)              // a becomes [1, 99, 3]

// slicing (start inclusive, end exclusive)
print(slice(a, 0, 2))     // [1, 99]

// iteration
for i = 0; i < len(a); i = i + 1 {
  print("a[" + to_string(i) + "] = " + to_string(a[i]))
}

// enumerate helper yields index/value pairs
#include <utils/iter.fun> as it  // typical place for helpers; adjust to your setup
for pair in it.enumerate(["x", "y"]) {
  idx = pair[0]
  val = pair[1]
  print(idx + ":" + val)
}
```

Notes:
- Indexing is bounds‑checked; invalid indices cause a runtime error that you can inspect with --trace or REPL‑on‑error.
- Arrays are mutable; operations modify in place unless documented otherwise.

## Maps

Create maps with curly braces. Keys are typically strings; values can be any type.

```
// creation
user = { "name": "Ada", "age": 37 }
cfg  = {}

print(typeof(user))        // "map"

// get/set by key
print(user["name"])       // Ada
user["lang"] = "en"

// overwrite existing key
user["age"] = 38

// nested access
book = { "title": "Fun Handbook", "meta": { "pages": 120 } }
print(book["meta"]["pages"])   // 120

// check presence via nil comparison (common pattern)
val = user["missing"]
if val == nil { print("no such key") }

// iterate known keys (maps are not ordered)
keys = ["name", "age", "lang"]
for i = 0; i < len(keys); i = i + 1 {
  k = keys[i]
  print(k + " = " + to_string(user[k]))
}
```

Notes:
- Accessing a non‑existing key returns nil; write a guard before using it as another type.
- Maps are mutable; assigning with map["k"] = v updates in place.

## Strings (brief)

```
s = "hello, world"
print(len(s))              // 12
print(substr(s, 7, 5))     // world
print(find(s, ","))       // 5 (index) or -1 if not found

parts = split("a,b,c", ",")  // ["a","b","c"]
print(join(parts, ";"))      // a;b;c
```

## Numbers and floats (brief)

```
n = 10
f = 3.14
print(n + 2)               // 12
print(f * 2)               // 6.28

// clamp to widths when needed
print(uclamp(300, 8))      // 44 (300 mod 256)
```

## Booleans and nil

```
ok = 1        // true
no = 0        // false
none = nil

if ok && !no { print("yay") }
if none == nil { print("is nil") }
```

## Conversions and typing

```
x = "42"
print(to_number(x) + 1)    // 43

print(typeof([1,2,3]))     // "array"
print(typeof({}))          // "map"
```

## Common patterns

- Accumulate values:
- 
  ```
  acc = []
  for i = 0; i < 5; i = i + 1 { push(acc, i*i) }
  print(acc)  // [0,1,4,9,16]
  ```

- Group related data with a map:
- 
  ```
  person = { "name": "Lin", "skills": ["C", "Fun"] }
  print(person["skills"][1])  // Fun
  ```

- Safe map read:
- 
  ```
  v = person["twitter"]
  handle = v != nil ? v : "(none)"
  print(handle)
  ```

## Interop highlights

- JSON (optional module) maps JSON arrays to Fun arrays and JSON objects to Fun maps.

  ```
  #include <json/json.fun>
  data = json_parse("{\"a\":[1,2]} ")
  print(typeof(data))       // map
  print(typeof(data["a"])) // array
  ```

## Troubleshooting

- Index errors on arrays typically mean you used a negative index or >= len(a).
- Map lookups that return nil: ensure the key exists and is spelled exactly.
- Converting types: prefer to_number/to_string; cast(x, "string") is also available for certain conversions.

## See also

- docs/handbook.md — “Core types and operations”, arrays/maps helpers
- docs/includes.md — how to include modules and use namespaces
- Examples in this repo:
  - examples/arrays.fun, examples/arrays_advanced.fun, examples/arrays_iter.fun
  - examples/maps.fun
  - examples/json/* (when JSON is enabled)
