#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-26
 */

// Example: Using the CRC32 class from lib/crypt/crc32.fun

include <crypt/crc32.fun>

print("-- CRC32 example --")

c = CRC32()

// 1) Known test vector: "123456789" -> cbf43926
msg = "123456789"
crc1 = c.crc32_str(msg)
print("Input (ASCII): " + msg)
print("CRC32: " + crc1)   // expected: cbf43926

print("")

// 2) Same data provided as hex string
hex_msg = "313233343536373839"  // hex for "123456789"
crc2 = c.crc32_hex(hex_msg)
print("Input (hex): " + hex_msg)
print("CRC32: " + crc2)   // expected: cbf43926

print("")

// 3) Another quick demo
other = "Fun language"
crc3 = c.crc32_str(other)
print("Input (ASCII): " + other)
print("CRC32: " + crc3)

/* Expected output:
-- CRC32 example --
Input (ASCII): 123456789
CRC32: cbf43926

Input (hex): 313233343536373839
CRC32: cbf43926

Input (ASCII): Fun language
CRC32: d7d83272
*/
