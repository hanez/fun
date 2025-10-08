#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Unsigned integer type examples with wrapping behavior

uint8 u8 = 255
uint16 u16 = 65535
uint32 u32 = 4294967295
uint64 u64 = 18446744073709551615  // will wrap to 64-bit

print("u8  = " + to_string(u8)  + " :: " + typeof(u8))
print("u16 = " + to_string(u16) + " :: " + typeof(u16))
print("u32 = " + to_string(u32) + " :: " + typeof(u32))
print("u64 = " + to_string(u64) + " :: " + typeof(u64))

// Demonstrate wrapping on assignment
u8 = 256        // wraps to 0
u16 = 65536      // wraps to 0
u32 = 4294967296 // wraps to 0
u64 = 18446744073709551616 // wraps to 0

print("wrap u8  -> " + to_string(u8))
print("wrap u16 -> " + to_string(u16))
print("wrap u32 -> " + to_string(u32))
print("wrap u64 -> " + to_string(u64))

// Mixed arithmetic then wrapping
u8 = 250
u8 = u8 + 10     // 260 -> wrap 260 % 256 = 4
print("u8 after +10 wrap -> " + to_string(u8))

// 'number' maps to 64-bit unsigned behavior
number n = -1    // will clamp/wrap to 64-bit (implementation-defined display)
print("number n = " + to_string(n) + " :: " + typeof(n))

/* Exprected result:
u8  = 255 :: Number
u16 = 65535 :: Number
u32 = 4294967295 :: Number
u64 = -1 :: Number
wrap u8  -> 0
wrap u16 -> 0
wrap u32 -> 0
wrap u64 -> 0
u8 after +10 wrap -> 4
number n = -1 :: Number
*/
