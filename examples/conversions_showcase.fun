#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-13
 */

// conversions_showcase.fun
// Demonstrates Fun's data type conversion features:
// - to_number(x)
// - to_string(x)
// - cast(value, typeName)
// - typeof(x)
// - uclamp(number, bits), sclamp(number, bits)

print("=== Conversions showcase ===")

// typeof on core literals
print("typeof(123) -> " + typeof(123))          // Number
print("typeof(\"abc\") -> " + typeof("abc"))  // String
print("typeof([1,2]) -> " + typeof([1,2]))     // Array
// Use a variable for map to avoid parser ambiguities
mm = { "a": 1 }
print("typeof({\"a\":1}) -> " + typeof(mm))     // Map
print("typeof(0) -> " + typeof(0))             // Number (Boolean is 0/1 as Number)
print("typeof(nil) -> " + typeof(nil))         // Nil

print("")
print("-- to_number(x) --")
print(to_number(42))            // 42
print(to_number("123"))        // 123
print(to_number("12x"))        // 0 (invalid -> 0)
print(to_number(0))             // 0
print(to_number(1))             // 1

print("")
print("-- to_string(x) --")
print(to_string(42))            // "42"
print(to_string("hi"))         // "hi"
print(to_string([1,2,3]))       // "[array n=3]" or similar representation
print(to_string({ "k": 7 }))        // "{\"k\":7}" or implementation-defined
print(to_string(nil))           // "nil" (implementation-defined)

print("")
print("-- cast(value, typeName) --")
// Number: parse decimals; invalid -> 0
print(cast("123", "Number"))
print(cast("12x", "Number"))

// String: stringify
print(cast(100, "String"))
print(typeof(cast(100, "String")))

// Boolean: 0 -> 0, non-zero -> 1
print(cast(0, "Boolean"))
print(cast(42, "Boolean"))

// Array: others get wrapped
a = cast(42, "Array")
print(typeof(a))
print(len(a))
print(a[0])

// Map: non-maps become empty map
m = cast(42, "Map")
print(typeof(m))

// Nil: always Nil
n = cast("x", "Nil")
print(typeof(n))

// Function: non-functions -> Nil
fun foo()
  return 7
print(typeof(cast(foo, "Function")))
print(typeof(cast(42, "Function")))

print("")
print("-- Integer width ranges via typed variables --")

// Unsigned 8-bit: values clamped to [0..255]
uint8 u8 = 0
u8 = 255
print(u8)          // -> 255
// u8 = -1        // Uncomment to see OverflowError: value out of range for uint8
// u8 = 300       // Uncomment to see OverflowError: value out of range for uint8

// Signed 8-bit: values clamped to [-128..127]
int8 s8 = 0
s8 = -128
print(s8)          // -> -128
s8 = 127
print(s8)          // -> 127
// s8 = -200      // Uncomment to see OverflowError for int8
// s8 = 200       // Uncomment to see OverflowError for int8

// 16-bit examples
uint16 u16 = 0
u16 = 65535
print(u16)         // -> 65535
// u16 = 70000    // Uncomment to see OverflowError for uint16

int16 s16 = 0
s16 = -32768
print(s16)         // -> -32768
s16 = 32767
print(s16)         // -> 32767

print("=== Done ===")

/* Expected output:
=== Conversions showcase ===
typeof(123) -> Number
typeof("abc") -> String
typeof([1,2]) -> Array
typeof({"a":1}) -> String
typeof(0) -> Number
typeof(nil) -> Nil

-- to_number(x) --
42
123
0
0
1

-- to_string(x) --
42
hi
[array n=3]
{map n=1}
nil

-- cast(value, typeName) --
123
0
100
String
0
1
Array
1
42
String
Nil
Function
Nil

-- Integer width ranges via typed variables --
255
-128
127
65535
-32768
32767
=== Done ===
*/
