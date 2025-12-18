#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-18
 */

/*
 * Example: Using hex utility functions with fixed values
 * This demonstrates hex_to_bytes, bytes_to_hex, and hex_to_dec.
 */

#include <hex.fun>

print("--- Hex utility demo (fixed values) ---")

// Using a fixed hex string for verification
hexstr = "50a76f143d67ce173962cd2eea5a86a4"
print("Fixed Hex string:")
print(hexstr)

print("Hex length (should be 32):")
print(to_string(len(hexstr)))

print("Hex bytes array:")
bytes = hex_to_bytes(hexstr)
print(to_string(bytes))

print("Hex dump to bytes:")
print(bytes)

print("Back to hex from bytes:")
print(bytes_to_hex(bytes))

print("Hex as decimal (first 4 bytes - 0x50a76f14):")
print(to_string(hex_to_dec(substr(hexstr, 0, 8))))

print("Hex as decimal (all bytes):")
print(to_string(hex_to_dec(hexstr)))

print("Decimal to hex (4135093009263527588):")
print(dec_to_hex(4135093009263527588))

print("Decimal to hex (1353150228):")
print(dec_to_hex(1353150228))

/* Expected output:
--- Hex utility demo (fixed values) ---
Fixed Hex string:
50a76f143d67ce173962cd2eea5a86a4
Hex length (should be 32):
32
Hex bytes array:
[array n=16]
Hex dump to bytes:[80, 167, 111, 20, 61, 103, 206, 23, 57, 98, 205, 46, 234, 90, 134, 164]
Back to hex from bytes:
50a76f143d67ce173962cd2eea5a86a4
Hex as decimal (first 4 bytes - 0x50a76f14):
1353150228
Hex as decimal (all bytes):
4135093009263527588
Decimal to hex (4135093009263527588):
3962cd2eea5a86a4
Decimal to hex (1353150228):
50a76f14
*/
