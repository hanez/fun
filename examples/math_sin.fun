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

// Demo of sin(x)

print("=== math sin demo start ===")

print("sin(0) -> " + to_string(sin(0)))
print("sin(1.57079632679) ~ pi/2 -> " + to_string(sin(1.57079632679)))
print("sin(-1.57079632679) -> " + to_string(sin(-1.57079632679)))

print("=== math sin demo end ===")

/* Expected output (approximate):
=== math sin demo start ===
sin(0) -> 0
sin(1.57079632679) ~ pi/2 -> 1
sin(-1.57079632679) -> -1
=== math sin demo end ===
*/
