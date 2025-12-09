#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-10
 */

// echo_example.fun
// Demonstrates echo(expr) which prints without a trailing newline.

// Build a line without newline using echo, then finish with print to add newline
echo("Hello, ")
echo("world")
print("!")

// Expected output:
// Hello, world!
