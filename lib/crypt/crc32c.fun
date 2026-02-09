/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-26
 */

// lib/crypt/crc32c.fun
// Pure Fun implementation of CRC-32C (Castagnoli) operating on hex-string input.
// Polynomial (reflected): 0x82F63B78
// Initial value: 0xFFFFFFFF, Final XOR: 0xFFFFFFFF
//
// Public API (class methods):
//   crc32c_hex(hexStr) -> 8-char lowercase hex string
//   crc32c_str(str)    -> 8-char lowercase string (ASCII input)
//
// Example:
//   // "123456789" in ASCII is 313233343536373839 in hex, CRC32C is e3069283
//   c = CRC32C()
//   print(c.crc32c_hex("313233343536373839"))

#include <hex.fun>
#include <strings.fun>

class CRC32C()
  // 32-bit helpers
  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  fun shr32(this, x, s)
    return shr(this.u32(x), s)

  fun shl32(this, x, s)
    return shl(this.u32(x), s)

  fun xor32(this, a, b)
    return bxor(this.u32(a), this.u32(b))

  fun and32(this, a, b)
    return band(this.u32(a), this.u32(b))

  // hex helpers (mirroring style from lib/crypt/md5.fun)
  fun from_hex(this, hex)
    arr = []
    i = 0
    n = len(hex)
    while i + 1 < n
      b = byte_from_hex_pair(substr(hex, i, 2))
      push(arr, b)
      i = i + 2
    return arr

  fun u32_to_hex8(this, n)
    // big-endian printing (MSB first), common for CRC displays
    b3 = this.and32(this.shr32(n, 24), 255)
    b2 = this.and32(this.shr32(n, 16), 255)
    b1 = this.and32(this.shr32(n, 8), 255)
    b0 = this.and32(n, 255)
    return bytes_to_hex([b3, b2, b1, b0])

  // Bitwise update (no table needed) using reflected polynomial 0x82F63B78
  POLY = 2197175160 // 0x82F63B78

  // Compute CRC32C over byte array, return u32 value (bitwise, reflected)
  fun crc32c_bytes_value(this, bytes)
    crc = 4294967295 // 0xFFFFFFFF
    i = 0
    n = len(bytes)
    while i < n
      crc = this.xor32(crc, bytes[i])
      j = 0
      while j < 8
        if (this.and32(crc, 1) == 1)
          crc = this.xor32(this.shr32(crc, 1), this.POLY)
        else
          crc = this.shr32(crc, 1)
        j = j + 1
      i = i + 1
    return this.xor32(crc, 4294967295) // final XOR

  // Public: compute CRC32C of hex string of bytes, return 8-char hex
  fun crc32c_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    v = this.crc32c_bytes_value(bytes)
    return this.u32_to_hex8(v)

  // Convenience: compute CRC32C of ASCII string
  fun crc32c_str(this, str)
    bytes = string_to_bytes_ascii(str)
    v = this.crc32c_bytes_value(bytes)
    return this.u32_to_hex8(v)
