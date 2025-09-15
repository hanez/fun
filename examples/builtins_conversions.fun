#!/usr/bin/env fun

// Conversions and length built-ins demo

// len on array and string
print(len([1, 2, 3]))          // -> 3
print(len("hello"))            // -> 5

// to_number with various string inputs
print(to_number("42"))         // -> 42
print(to_number("   -7  "))    // -> -7
print(to_number("12a"))        // non-numeric suffix -> 0
print(to_number(""))           // empty -> 0
print(to_number("  "))         // spaces -> 0

// to_string on numbers and arrays
print(to_string(42))           // -> "42" (printed as 42)

arr = [1, 2]
s = to_string(arr)             // -> "[array n=2]"
print(s)

// combine to_string with concatenation
print("val=" + to_string(99))  // -> "val=99"

/* Expected output:
3
5
42
-7
0
0
0
42
[array n=2]
val=99
*/
