#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

// OpenSSL MD5 example
// Enable with -DFUN_WITH_OPENSSL=ON during build for real hashing.

s = "abc"
d = openssl_md5(s)
print("md5(abc) = " + d)

// Another quick check (empty string)
e = ""
print("md5(\"\") = " + openssl_md5(e))

/* Expected output:
md5(abc) = 900150983cd24fb0d6963f7d28e17f72
md5("") = d41d8cd98f00b204e9800998ecf8427e
*/
