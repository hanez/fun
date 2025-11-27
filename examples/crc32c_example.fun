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

// Example: Using the CRC32C class from lib/crypt/crc32c.fun

include <crypt/crc32c.fun>

print("-- CRC32C example --")

c = CRC32C()

// 1) Known test vector: "123456789" -> e3069283
msg = "123456789"
crc1 = c.crc32c_str(msg)
print("Input (ASCII): " + msg)
print("CRC32C: " + crc1)   // expected: e3069283

print("")

// 2) Same data provided as hex string
hex_msg = "313233343536373839"  // hex for "123456789"
crc2 = c.crc32c_hex(hex_msg)
print("Input (hex): " + hex_msg)
print("CRC32C: " + crc2)   // expected: e3069283

print("")

// 3) Another quick demo
other = "Fun language"
crc3 = c.crc32c_str(other)
print("Input (ASCII): " + other)
print("CRC32C: " + crc3)

/* Expected output:
-- CRC32C example --
Input (ASCII): 123456789
CRC32C: e3069283

Input (hex): 313233343536373839
CRC32C: e3069283

Input (ASCII): Fun language
CRC32C: c0158b58
*/
