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

print("=== math gcd/lcm demo start ===")

print("gcd(48,18) -> " + to_string(gcd(48,18)))   // 6
print("gcd(0,5) -> " + to_string(gcd(0,5)))       // 5
print("gcd(-21,6) -> " + to_string(gcd(-21,6)))  // 3

print("lcm(21,6) -> " + to_string(lcm(21,6)))     // 42
print("lcm(0,5) -> " + to_string(lcm(0,5)))       // 0
print("lcm(-4,6) -> " + to_string(lcm(-4,6)))     // 12

print("=== math gcd/lcm demo end ===")

/* Expected output (values):
=== math gcd/lcm demo start ===
gcd(48,18) -> 6
gcd(0,5) -> 5
gcd(-21,6) -> 3
lcm(21,6) -> 42
lcm(0,5) -> 0
lcm(-4,6) -> 12
=== math gcd/lcm demo end ===
*/
