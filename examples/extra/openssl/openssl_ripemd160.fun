#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

// OpenSSL RIPEMD-160 example
// Enable with -DFUN_WITH_OPENSSL=ON during build for real hashing.

s = "abc"
d = openssl_ripemd160(s)
print("ripemd160(abc) = " + d)

// Another quick check (empty string)
e = ""
print("ripemd160(\"\") = " + openssl_ripemd160(e))

/* Expected output (if RIPEMD-160 is available in your OpenSSL build):
ripemd160(abc) = 8eb208f7e05d987a9b044a8e98c6b087f15a0bfc
ripemd160("") = 9c1185a5c5e9fc54612808977ee8f548b2258d31
*/
