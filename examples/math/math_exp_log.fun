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

// Demo of exp(x), log(x), log10(x)

print("=== math exp/log demo start ===")

print("exp(0) -> " + to_string(exp(0)))
print("log(1) -> " + to_string(log(1)))
print("log10(1) -> " + to_string(log10(1)))
print("exp(1) -> " + to_string(exp(1)))
print("log(2.71828182846) ~ ln(e) -> " + to_string(log(2.71828182846)))

print("=== math exp/log demo end ===")

/* Expected output (approximate):
=== math exp/log demo start ===
exp(0) -> 1
log(1) -> 0
log10(1) -> 0
exp(1) -> 2.718281828...
log(2.71828182846) ~ ln(e) -> 1
=== math exp/log demo end ===
*/
