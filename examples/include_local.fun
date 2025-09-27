#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// To test this you need to go to ./examples/ and test includes running ../build/fun include_local.fun
// Local includes are always relative from the $PWD.

// Demonstrates local include:
// This resolves relative to the current working directory where 'fun' is executed.
#include "include_local_util.fun"

print("== include local demo ==")
greet("Fun")

number a = 2
number b = 3
print("sum(" + to_string(a) + ", " + to_string(b) + ") = " + to_string(sum(a, b)))

// Tip:
// To include from the system library directory (/usr/lib/fun), use angle brackets:
//   #include <some_lib.fun>

