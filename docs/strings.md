# Working with strings in Fun

This guide covers string literals, common operations (length, concatenation, substring, search, split), and interop patterns. Strings in Fun are immutable sequences of bytes/text.

## TL;DR

- Create with quotes: s = "hello". Escape with \n, \t, \", \\.
- Concatenate with +. Convert non-strings with to_string(x).
- Length: len(s). Substring: substr(s, start, length).
- Find index: find(s, needle) → 0-based index or -1 if not found.
- Split CSV: split("a,b,c", ",") → ["a","b","c"].

## Literals and escaping

```
s1 = "hello"
s2 = "line1\nline2"   // newline
s3 = "quote: \" and backslash: \\" // escaped quote and backslash

print(s1)  // hello
```

Notes:
- Strings are immutable; operations return new strings rather than modifying in place.
- Use to_string(x) when concatenating non-string values.

## Basic operations

Length and concatenation:

```
name = "Ada"
greet = "Hello, " + name + "!"    // "Hello, Ada!"
print(len(greet))                   // 12
```

Substring (start, length) and search:

```
s = "hello, world"
print(substr(s, 7, 5))   // world
idx = find(s, ",")      // 5, or -1 if not found
if idx >= 0 { print("comma at index " + to_string(idx)) }
```

Splitting into arrays:

```
parts = split("a,b,c", ",")  // ["a","b","c"]
for i = 0; i < len(parts); i = i + 1 {
  print(parts[i])
}
```

## Conversions and formatting

```
n = 42
pi = 3.14
msg = "n=" + to_string(n) + ", pi=" + to_string(pi)
print(msg)

// parsing (may error if the string is not numeric)
n2 = to_number("123")    // 123
```

If you need a specific type, you can use cast for advanced cases, e.g. cast("123", "number").

## Common patterns

- Guard on find results before slicing:

```
email = "user@example.org"
at = find(email, "@")
if at >= 0 {
  user = substr(email, 0, at)
  host = substr(email, at + 1, len(email) - at - 1)
  print(user + " on " + host)
}
```

- Building paths or messages:

```
base = "/tmp"
file = "log.txt"
path = base + "/" + file
```

## Gotchas

- Strings are immutable: repeated concatenation in big loops can be costly; consider collecting pieces in an array and joining at the end if you have a helper for that in your setup.
- len(s) counts bytes/code units; be mindful when working with multi-byte encodings.

## See also

- Core overview: [types.md](./types.md)
- Arrays guide (useful when splitting/collecting text): [arrays.md](./arrays.md)
