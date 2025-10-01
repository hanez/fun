#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

/*
 * SHA-1 usage demo
 *
 * Run without installing:
 *   FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/sha1_demo.fun
 */

#include <crypt/sha1.fun> as sha1

print("=== SHA-1 demo ===")

s = "abc"
hex_abc = "616263"   // "abc" in hex
empty = ""

// Hash ASCII string directly
d1 = sha1.SHA1().sha1_str(s)
print("SHA-1('abc')        = " + d1)

// Hash pre-encoded hex bytes
d2 = sha1.SHA1().sha1_hex(hex_abc)
print("SHA-1(616263 hex)   = " + d2)

// Empty string
d3 = sha1.SHA1().sha1_str(empty)
print("SHA-1('')           = " + d3)

print("=== done ===")

/* Expected output:
=== SHA-1 demo ===
SHA-1('abc')        = e6cd2bcee460c45d41565ac877d866a159a16e19
SHA-1(616263 hex)   = e6cd2bcee460c45d41565ac877d866a159a16e19
SHA-1('')           = da39a3ee5e6b4b0d3255bfef95601890afd80709
=== done ===
