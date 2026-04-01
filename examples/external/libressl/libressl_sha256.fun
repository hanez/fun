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

// LibreSSL SHA-256 example
// Enable with -DFUN_WITH_LIBRESSL=ON during build for real hashing.

s = "abc"
d = libressl_sha256(s)
print("sha256(abc) = " + d)

// Another quick check (empty string)
e = ""
print("sha256(\"\") = " + libressl_sha256(e))

/* Expected output:
sha256(abc) = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
sha256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
*/
