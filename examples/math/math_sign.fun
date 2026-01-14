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

print("=== math sign demo start ===")

print("sign(-5) -> " + to_string(sign(-5)))     // -1
print("sign(0) -> " + to_string(sign(0)))       // 0
print("sign(7) -> " + to_string(sign(7)))       // 1
print("sign(-0.0) -> " + to_string(sign(-0.0))) // 0
print("sign(3.14) -> " + to_string(sign(3.14))) // 1

print("=== math sign demo end ===")

/* Expected output (values):
=== math sign demo start ===
sign(-5) -> -1
sign(0) -> 0
sign(7) -> 1
sign(-0.0) -> 0
sign(3.14) -> 1
=== math sign demo end ===
*/
