#!/usr/bin/env fun

/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

print("Reassigning typed variables to another type should fail")

string s = "hello"
print(typeof(s))   // -> "String"
print(s)           // -> "hello"

print("Now trying to assign a number to a string variable...")
s = 42             // Runtime TypeError: expected String (assignment rejected)

print("This line will not execute due to the type error")
