#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 *  Added: 2026-01-03
 */

print("=== math isqrt demo start ===")

print("isqrt(-4) -> " + to_string(isqrt(-4)))    // 0 (non-negative only)
print("isqrt(0) -> " + to_string(isqrt(0)))      // 0
print("isqrt(1) -> " + to_string(isqrt(1)))      // 1
print("isqrt(2) -> " + to_string(isqrt(2)))      // 1
print("isqrt(3) -> " + to_string(isqrt(3)))      // 1
print("isqrt(4) -> " + to_string(isqrt(4)))      // 2
print("isqrt(15) -> " + to_string(isqrt(15)))    // 3
print("isqrt(16) -> " + to_string(isqrt(16)))    // 4

print("=== math isqrt demo end ===")

/* Expected output (values):
=== math isqrt demo start ===
isqrt(-4) -> 0
isqrt(0) -> 0
isqrt(1) -> 1
isqrt(2) -> 1
isqrt(3) -> 1
isqrt(4) -> 2
isqrt(15) -> 3
isqrt(16) -> 4
=== math isqrt demo end ===
*/
