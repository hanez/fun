# Maps in Fun

This guide focuses on maps: creation, reading/writing by key, checking key presence, iterating keys/values, nesting, and common gotchas. It complements the quick overview in types.md with a deeper, example‑driven treatment.

## What is a map?

- Associative container of key → value pairs.
- Keys are typically strings; values can be any type (numbers, strings, arrays, maps, …).
- Mutable: assigning with m["k"] = v updates the map in place.
- Accessing a missing key yields nil (not an error). Use guards or has(m, key).
- Maps are not ordered; rely on keys(m) if you need a concrete list of keys.

## Creating maps

```
// literals
user = { "name": "Ada", "age": 37 }
empty = {}

print(typeof(user))     // "map"
```

Nested structures are natural and common:

```
book = {
  "title": "Fun Handbook",
  "meta": { "pages": 120, "isbn": "123-456" },
  "tags": ["lang", "vm"]
}
print(book["meta"]["pages"])   // 120
```

## Getting and setting by key

```
profile = { "name": "Lin" }

// read existing key
print(profile["name"])       // Lin

// read missing key → nil
print(profile["email"])      // nil

// write / overwrite
profile["email"] = "lin@example.org"
profile["name"]  = "Linus"
print(profile)                 // {"name":"Linus","email":"lin@example.org"}
```

Notes:
- Using a non‑string key is allowed only if your build/runtime supports it; most code uses string keys for portability.
- Missing keys produce nil. Compare against nil before converting or indexing:

```
v = profile["phone"]
if v == nil { print("no phone on file") }
```

## Checking key existence

Use has(m, key) to check if a key is present (returns 1 or 0):

```
cfg = { "debug": 1 }
print(has(cfg, "debug"))   // 1
print(has(cfg, "port"))    // 0

if has(cfg, "port") { print(cfg["port"]) } else { print("using default port") }
```

## Iterating maps

Maps are not inherently ordered. To iterate, first obtain an array of keys or values.

```
user = { "name": "Ada", "age": 38 }

// iterate known keys (explicit order you choose)
order = ["name", "age"]
for i = 0; i < len(order); i = i + 1 {
  k = order[i]
  print(k + " = " + to_string(user[k]))
}

// discover keys from the map (order may depend on implementation)
ks = keys(user)           // -> ["name", "age"] (example)
for i = 0; i < len(ks); i = i + 1 {
  k = ks[i]
  print(k + ": " + to_string(user[k]))
}

// values only
vs = values(user)         // -> ["Ada", 38]
for i = 0; i < len(vs); i = i + 1 { print(to_string(vs[i])) }
```

Tip:
- If you need deterministic output, either define the order array explicitly or sort the result of keys(user) using your available utilities before looping.

## Copying vs. referencing

Maps are reference types. Assigning copies the reference, not the contents:

```
orig = { "a": 1 }
alias = orig
alias["a"] = 9
print(orig["a"])   // 9

// To make a shallow copy, rebuild from keys/values
src = { "x": 1, "y": 2 }
dst = {}
ks  = keys(src)
for i = 0; i < len(ks); i = i + 1 { k = ks[i]; dst[k] = src[k] }

dst["x"] = 7
print(src["x"])     // 1
print(dst["x"])     // 7
```

Shallow copies duplicate only the top‑level mapping; nested arrays/maps inside are still shared unless you clone them manually.

## Equality

```
print({"a":1,"b":2} == {"b":2,"a":1})  // true
print({"a":1} == {"a":2})                // false
```

Map equality compares sets of keys and their corresponding values for equality (order does not matter).

## Common utilities

Depending on your build/stdlib configuration, these helpers are commonly available:

- has(m, key): 1 if present, 0 otherwise
- keys(m): array of keys
- values(m): array of values (parallel to keys(m) order)
- len(a) works on arrays; for maps, prefer keys(m) then len(keys(m)) if you need a count

Check your lib or VM docs (e.g., src/vm/maps) and docs/types.md for availability and details.

## Interop with arrays and strings

```
// maps inside arrays
users = [ {"name":"Ada"}, {"name":"Lin"} ]
for i = 0; i < len(users); i = i + 1 {
  print(users[i]["name"])   // Ada, Lin
}

// arrays inside maps
m = { "nums": [1,2,3] }
arr = m["nums"]
push(arr, 4)
print(m["nums"])           // [1,2,3,4]

// JSON interop is typically via lib/io/json.fun (if enabled in your build)
#include <io/json.fun> as json    // adjust to your tree and build flags
s = json.stringify({"ok":1})     // "{"ok":1}"
```

## Error handling and edge cases

- Accessing a missing key returns nil. Guard before arithmetic or nested indexing.
- Mutating a map while iterating over keys you computed earlier is safe, but remember the keys array won’t update automatically; recompute if needed.
- Treat map iteration order as unspecified unless your build guarantees stability. For user‑facing output, specify/derive an order explicitly.

## Performance tips

- If you will access many keys, cache ks = keys(m) once and index that array in a loop rather than calling keys(m) repeatedly.
- Prefer direct writes (m["k"]=v) over repeatedly rebuilding maps in tight loops.
- When copying, copy only required keys instead of cloning the entire map if you only need a subset.

## Examples

```
// merge defaults into config (without overwriting explicit keys)
defaults = { "host":"127.0.0.1", "port":8080, "debug":0 }
cfg      = { "port": 9000 }

ks = keys(defaults)
for i = 0; i < len(ks); i = i + 1 {
  k = ks[i]
  if !has(cfg, k) { cfg[k] = defaults[k] }
}
print(cfg)  // {"port":9000,"host":"127.0.0.1","debug":0}

// index users by id
rows = [ {"id":"u1","name":"Ada"}, {"id":"u2","name":"Lin"} ]
by_id = {}
for i = 0; i < len(rows); i = i + 1 {
  r = rows[i]
  by_id[r["id"]] = r
}
print(by_id["u2"]["name"])  // Lin
```

## See also

- types.md — broader overview of core types with quick map examples.
- examples/ — there are example scripts involving maps; try play.fun to explore.
