#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Demonstrates system library includes after installation to /usr/lib/fun

// includes can also be done with a leading # like in C, but some programmers maybe like more clean code without a 
// leading #.
include <hello.fun>
include <utils/math.fun>

print("== include lib demo ==")
hello_lib()

number x = 10
number y = 32

print("add(" + to_string(x) + ", " + to_string(y) + ") = " + to_string(add(x, y)))
print("times(" + to_string(x) + ", " + to_string(y) + ") = " + to_string(times(x, y)))
