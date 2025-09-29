#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 *
 * Added: 2025-09-29
 */

// Include the pure Fun CRC32 library
include "lib/crypt/crc32.fun"

// Create a Class-typed instance to prevent accidental shadowing
Class c32 = CRC32()
print(typeof(c32))  // should be "Class"

print("=== CRC32 demo (hex input) ===")

// "" (empty) -> 00000000
print(c32.crc32_hex(""))

 // "abc" -> 352441c2
print(c32.crc32_hex("616263"))

// "123456789" -> cbf43926
print(c32.crc32_hex("313233343536373839"))

print("=== Done ===")
