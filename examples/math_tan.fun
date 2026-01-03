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

// Demo of tan(x)

print("=== math tan demo start ===")

print("tan(0) -> " + to_string(tan(0)))
print("tan(0.78539816339) ~ pi/4 -> " + to_string(tan(0.78539816339)))

print("=== math tan demo end ===")

/* Expected output (approximate):
=== math tan demo start ===
tan(0) -> 0
tan(0.78539816339) ~ pi/4 -> 1
=== math tan demo end ===
*/
