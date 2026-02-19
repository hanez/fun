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

// LibreSSL RIPEMD-160 example
// Enable with -DFUN_WITH_LIBRESSL=ON during build for real hashing.

s = "abc"
d = libressl_ripemd160(s)
print("ripemd160(abc) = " + d)

// Another quick check (empty string)
e = ""
print("ripemd160(\"\") = " + libressl_ripemd160(e))

/* Expected output:
ripemd160(abc) = 8eb208f7e05d987a9b049a9a5c0c2b74e07e6a5d
ripemd160("") = 9c1185a5c5e9fc54612808977ee8f548b2258d31
*/
