# Arrays in Fun

This guide focuses on arrays: creation, indexing, mutation, iteration, slicing, and common gotchas. It complements the quick overview in types.md with a deeper, example‑driven treatment.

## What is an array?

- Ordered, zero‑indexed, mutable sequence of values.
- Can hold mixed types (numbers, strings, maps, arrays, …) in the same array.
- Bounds‑checked indexing; out‑of‑range access is a runtime error.

## Creating arrays

```
// literals
a = [1, 2, 3]
b = ["alpha", "beta"]
empty = []

// nested
grid = [[1,2], [3,4]]

print(typeof(a))   // "array"
print(len(a))      // 3
```

Tip: Prefer square‑bracket literals for clarity and performance versus building via repeated push in a hot loop.

## Indexing (0‑based) and assignment

```
a = [10, 20, 30]
print(a[0])   // 10
print(a[2])   // 30

// update in place
a[1] = 42
print(a)      // [10, 42, 30]
```

Notes:
- Valid indices are 0..len(a)-1. Using an invalid index raises a runtime error.
- Assignment updates the existing array; references pointing to it observe the change.

## Appending, popping, inserting, removing

```
a = [1]

// append to end; returns new length
push(a, 7)          // => 2, a is now [1, 7]

// pop last element; returns the removed value
v = apop(a)         // v = 7, a is now [1]

// insert at index (shifts elements to the right)
a = [1, 2, 3]
insert(a, 1, 99)    // a => [1, 99, 2, 3]

// remove at index (shifts left)
remove(a, 2)        // a => [1, 99, 3]
```

## Slicing and concatenation

```
a = [0,1,2,3,4]

// slice(startInclusive, endExclusive)
head = slice(a, 0, 3)   // [0,1,2]
mid  = slice(a, 1, 4)   // [1,2,3]

// concat: join two arrays
b = ["x", "y"]
ab = concat(a, b)       // [0,1,2,3,4,"x","y"]
```

Slicing returns a new array. The original is unchanged.

## Iteration patterns

```
a = ["a", "b", "c"]

// index‑based loop
for i = 0; i < len(a); i = i + 1 {
  print(a[i])
}

// enumerate helper (if available in your stdlib setup)
#include <utils/iter.fun> as it
for pair in it.enumerate(a) {
  idx = pair[0]
  val = pair[1]
  print(to_string(idx) + ":" + val)
}
```

## Copying vs. referencing

Arrays are reference types. Assigning just copies the reference, not the contents:

```
orig = [1, 2]
alias = orig        // points to the same array
alias[0] = 9
print(orig)         // [9, 2]

// create a shallow copy via slice
copy = slice(orig, 0, len(orig))
copy[1] = 7
print(orig)         // [9, 2]
print(copy)         // [9, 7]
```

Shallow copies duplicate the top‑level array but not nested structures.

## Equality

```
print([1,2] == [1,2])   // true
print([1,2] == [2,1])   // false
```

Array equality compares length and element‑wise equality recursively.

## Common utilities

Depending on your build/stdlib configuration, these helpers may be available:

- len(a): number of elements
- push(a, v), apop(a)
- insert(a, idx, v), remove(a, idx)
- slice(a, start, end)
- concat(a, b)
- find(a, v): index or -1
- contains(a, v): 1 or 0

Check your lib directory (e.g., lib/utils) for additional helpers.

## Error handling and bounds

```
a = [0]
// a[1] is out of range → runtime error
```

Tips:
- Guard indices: if i < 0 or i >= len(a) { /* handle */ }
- Use remove/insert carefully inside loops; indices of following items change.

## Interop with maps and strings

```
// arrays of maps
users = [ {"name":"Ada"}, {"name":"Lin"} ]
print(users[1]["name"])    // Lin

// split/join patterns depend on your stdlib
#include <utils/strings.fun> as su  // adjust if present in your tree
parts = su.split("a,b,c", ",")    // ["a","b","c"]
csv   = su.join(parts, ",")       // "a,b,c"
```

## Performance tips

- Preallocate by building from literals or chunked appends rather than one‑by‑one in very tight loops.
- Prefer index loops over repeated remove/insert in the middle of large arrays.
- Use slice to copy only when necessary; keep references for read‑only sharing.

## Examples

```
// filter even numbers
src = [0,1,2,3,4,5]
dst = []
for i = 0; i < len(src); i = i + 1 {
  v = src[i]
  if v % 2 == 0 { push(dst, v) }
}
print(dst)  // [0,2,4]

// flatten one level
nested = [[1,2], [3], [], [4,5]]
flat = []
for i = 0; i < len(nested); i = i + 1 {
  row = nested[i]
  for j = 0; j < len(row); j = j + 1 {
    push(flat, row[j])
  }
}
print(flat) // [1,2,3,4,5]
```

## See also

- types.md — broader overview of core types with quick array examples.
- examples/ — many scripts operate on arrays; try play.fun to explore.
