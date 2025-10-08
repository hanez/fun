#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// To test this you need to go to ./examples/ and test includes running ../build/fun include_local.fun
// Local includes are always relative from the $PWD using "" and system includes are located at /usr/lib/fun using <>.

// Demonstrates local include:
// This resolves relative to the current working directory where 'fun' is executed.
#include "examples/include_local_util.fun"

print("== include local demo ==")
greet("Fun")

number a = 2
number b = 3
print("sum(" + to_string(a) + ", " + to_string(b) + ") = " + to_string(sum(a, b)))

// Tip:
// To include from the system library directory (/usr/lib/fun), use angle brackets:
//   #include <some_lib.fun>

