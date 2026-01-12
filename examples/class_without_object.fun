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

// Demonstrates the usage of functions in classes without initiating an object.

include <crypt/sha256.fun>

print(SHA256().sha256_hex(""))

/* Expected output:
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
*/
