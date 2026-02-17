# Working with numbers and floats in Fun

This guide covers the numeric types in Fun, with a focus on the integer "number" type and the 64‑bit floating point "float" type. You’ll find creation, arithmetic, conversion, clamping, bitwise operations, and common patterns.

## TL;DR

- number = signed integer; float = 64‑bit floating point.
- Use +, -, *, / for arithmetic. If you need fractional results, make at least one operand a float.
- Modulo: a % b. Integer division with / may discard the fractional part; cast to float to preserve it.
- Convert/parse: to_number("123"), to_string(x), cast(x, "float"), cast(x, "number").
- Clamp to widths when interfacing with external code: uclamp(n, bits), sclamp(n, bits).
- Bitwise (numbers only): &, |, ^, ~, <<, >>.

## Numeric types at a glance

- number: signed integer (implementation‑defined width; use uclamp/sclamp for fixed‑width interop)
- float: IEEE‑754 double precision (64‑bit)

```
an = 42            // number
af = 3.14159       // float
print(typeof(an))  // "number"
print(typeof(af))  // "float"
```

## Literals

- Integer (number): 0, 1, -7, 120
- Floating point (float): 0.0, 1.5, -2.75, 1e3, -4.2e-1

```
x = 10
y = 2.5
z = -3
```

## Arithmetic

Basic arithmetic works as you’d expect:

```
a = 7
b = 2
print(a + b)   // 9
print(a - b)   // 5
print(a * b)   // 14
print(a % b)   // 1 (modulo)
```

Division and result type:

```
// If you need a fractional result, ensure a float is involved
print(7 / 2)                    // implementation may yield 3 or 3.5 depending on numeric rules
print(cast(7, "float") / 2)    // 3.5 (recommended when you need fractions)
print(7 / 2.0)                  // 3.5
```

Mixing numbers and floats promotes the operation to float semantics:

```
print(2 + 0.5)   // 2.5
```

## Comparisons

```
print(3 < 5)     // 1 (true)
print(3 == 3)    // 1
print(3 != 4)    // 1

// Be explicit when comparing ints vs floats if types matter
print(1 == 1.0)              // may be true, but types differ
print(cast(1.0, "number") == 1) // 1 (true) with explicit cast
```

## Conversions and parsing

```
n = to_number("123")        // 123 (number)
f = cast(n, "float")        // 123.0 (float)
n2 = cast(3.9, "number")    // 3 (truncation semantics)
print(to_string(f))          // "123"
```

If parsing fails (e.g., to_number("abc")), expect a runtime error; guard accordingly.

## Clamping to fixed widths

When interoperating with bytecode, C APIs, or binary formats, clamp integers to a specific bit width.

```
// Unsigned clamp to N bits
u8  = uclamp(300, 8)   // 44
u16 = uclamp(70000, 16)

// Signed clamp to N bits
s8  = sclamp(-130, 8)  // wraps into signed 8‑bit range
```

Choose the bits according to the target field (8, 16, 32, 64). See your interop API docs for exact ranges.

## Bitwise operations (numbers)

Bitwise operators apply to the integer number type.

```
a = 0b0110  // if binary literals aren’t supported in your setup, use decimals: a = 6
b = 0b0011  // or b = 3

print(a & b)  // 0b0010 -> 2
print(a | b)  // 0b0111 -> 7
print(a ^ b)  // 0b0101 -> 5
print(~a)     // bitwise NOT (two’s complement rules)
print(a << 1) // 12
print(a >> 1) // 3
```

Note: Bitwise ops are defined for numbers, not floats. Cast floats to numbers first when needed.

## Common patterns

Ensuring float math to avoid unintended truncation:

```
avg = cast(sum, "float") / cast(count, "float")
```

Safe division with guard against zero:

```
num = 10
den = 0
if den == 0 {
  print("division by zero")
} else {
  print(num / den)
}
```

Parsing user input with fallback:

```
raw = "not-a-number"
val = 0
// simplistic guard pattern; adapt to your error handling style
if find(raw, "0") >= 0 || find(raw, "1") >= 0 {  // crude pre-check
  val = to_number(raw)
}
```

## Gotchas

- Integer division vs float division: promote to float when you need fractional results.
- Overflow/underflow: clamp explicitly when targeting fixed‑width fields; otherwise values follow the VM’s integer semantics.
- Bitwise with negatives uses two’s complement; ~x equals -(x+1).

## See also

- Core overview: [types.md](./types.md)
- Math helpers and advanced ops: check [math/opcodes](./opcodes.md) and the vm/math sources for available functions.
- Strings: [strings.md](./strings.md)
