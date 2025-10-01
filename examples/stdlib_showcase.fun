#!/usr/bin/env fun

/*
 * Standard library showcase example for arrays, strings, math, ranges, and base64.
 */

#include <arrays.fun>
#include <strings.fun>
#include <math.fun>
#include <utils/range.fun>
#include <encoding/base64.fun>

print("=== Arrays ===")
arr = [1,2,2,3,4]
print(join(array_slice(arr, 1, 3), ","))      // "2,2,3"
print(join(array_reverse(arr), ","))          // "4,3,2,2,1"
print(array_index_of(arr, 3))                 // 3's index
print(array_contains(arr, 5))                 // 0
print(join(array_unique(arr), ","))           // "1,2,3,4"
print(join(array_flatten1([[1,2],[3],[4,5]]), ",")) // "1,2,3,4,5"

print("=== Strings ===")
s = "  Hello World \n"
print("["+str_trim(s)+"]")                     // "[Hello World]"
print(str_starts_with("foobar", "foo"))        // 1
print(str_ends_with("foobar", "bar"))          // 1
parts = str_split("a,b,c", ",")
print(join(parts, "|"))                        // "a|b|c"
print(str_replace_all("banana", "na", "NA"))   // "baNANA"
print(str_to_lower("FUN"))                     // "fun"
print(str_to_upper("Fun"))                     // "FUN"
print(str_repeat("ha", 3))                     // "hahaha"

print("=== Math ===")
print(abs(-5))                                 // 5
print(clamp(42, 0, 10))                        // 10
print(gcd(54, 24))                             // 6
print(lcm(21, 6))                              // 42
print(powi(3, 5))                              // 243
print(array_min([9,2,8,3]))                    // 2
print(array_max([9,2,8,3]))                    // 9
print(min3(3,1,2))                             // 1
print(max3(3,1,2))                             // 3

print("=== Range ===")
print(join(range(5), ","))                     // "0,1,2,3,4"
print(join(range2(3, 8), ","))                 // "3,4,5,6,7"
print(join(range3(10, 0, -3), ","))            // "10,7,4,1"

print("=== Base64 ===")
bytes = [0x48,0x65,0x6c,0x6c,0x6f]             // "Hello"
b64 = b64_encode_bytes(bytes)
print(b64)                                     // "SGVsbG8="
print(join(b64_decode_to_bytes(b64), ","))     // "72,101,108,108,111"
