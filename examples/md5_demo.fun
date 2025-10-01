#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

// Include the pure Fun MD5 library
include <crypt/md5.fun>

// create a hasher instance
md5 = MD5()

print("=== MD5 demo (hex input) ===")

// "abc" => 0x61 0x62 0x63
print(md5.md5_hex("616263"))            // -> 900150983cd24fb0d6963f7d28e17f72

// empty string "" => hex ""
print(md5.md5_hex(""))                  // -> d41d8cd98f00b204e9800998ecf8427e

// "message digest"
print(md5.md5_hex("6d65737361676520646967657374"))  // -> f96b697d7cb7938d525a2f31aaf161d0

// "abcdefghijklmnopqrstuvwxyz"
print(md5.md5_hex("6162636465666768696a6b6c6d6e6f707172737475767778797a")) // -> c3fcd3d76192e4007dfb496cca67e13b

// "Have Fun!"
print(md5.md5_str("Have Fun!")) // -> 852438d026c018c4307b916406f98c62

print("=== Done ===")

/* Expected output:
=== MD5 demo (hex input) ===
900150983cd24fb0d6963f7d28e17f72
d41d8cd98f00b204e9800998ecf8427e
f96b697d7cb7938d525a2f31aaf161d0
c3fcd3d76192e4007dfb496cca67e13b
812f2c01287af0e7c0a0b3daa381a51a
=== Done ===
*/
