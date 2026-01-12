#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-12
 */

// Demonstrates the the console prompt.

include <io/console.fun>

input = Console().prompt("fun> ")
print("You entered: " + input)

/* Possible output:
fun> Fun!
You entered: Fun!
*/
