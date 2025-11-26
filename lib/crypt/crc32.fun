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

// lib/crypt/crc32.fun
// Pure Fun implementation of CRC-32 (IEEE 802.3) operating on hex-string input.
// Reflected polynomial: 0xEDB88320
// Initial value: 0xFFFFFFFF, Final XOR: 0xFFFFFFFF
//
// Public API (class methods):
//   crc32_hex(hexStr) -> 8-char lowercase hex string
//   crc32_str(str)    -> 8-char lowercase string (ASCII input)
//
// Example:
//   // "123456789" CRC32 is cbf43926
//   c = CRC32()
//   print(c.crc32_str("123456789"))

#include <strings.fun>

class CRC32()
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
  fun hex_val(this, ch)
    if (ch == "0")
      return 0
    else if (ch == "1")
      return 1
    else if (ch == "2")
      return 2
    else if (ch == "3")
      return 3
    else if (ch == "4")
      return 4
    else if (ch == "5")
      return 5
    else if (ch == "6")
      return 6
    else if (ch == "7")
      return 7
    else if (ch == "8")
      return 8
    else if (ch == "9")
      return 9
    else if (ch == "a" || ch == "A")
      return 10
    else if (ch == "b" || ch == "B")
      return 11
    else if (ch == "c" || ch == "C")
      return 12
    else if (ch == "d" || ch == "D")
      return 13
    else if (ch == "e" || ch == "E")
      return 14
    else if (ch == "f" || ch == "F")
      return 15
    else
      return 0

  fun byte_from_hex_pair(this, hh)
    hi = this.hex_val(substr(hh, 0, 1))
    lo = this.hex_val(substr(hh, 1, 1))
    return hi * 16 + lo

  fun from_hex(this, hex)
    arr = []
    i = 0
    n = len(hex)
    while i + 1 < n
      b = this.byte_from_hex_pair(substr(hex, i, 2))
      push(arr, b)
      i = i + 2
    return arr

  fun two_hex(this, n)
    n = n % 256
    d = ["0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"]
    hi = n / 16
    lo = n % 16
    parts = [d[hi], d[lo]]
    return join(parts, "")

  fun bytes_to_hex(this, arr)
    i = 0
    out = []
    while i < len(arr)
      push(out, this.two_hex(arr[i]))
      i = i + 1
    return join(out, "")

  fun u32_to_hex8(this, n)
    // big-endian printing (MSB first), common for CRC displays
    b3 = this.and32(this.shr32(n, 24), 255)
    b2 = this.and32(this.shr32(n, 16), 255)
    b1 = this.and32(this.shr32(n, 8), 255)
    b0 = this.and32(n, 255)
    return this.bytes_to_hex([b3, b2, b1, b0])

  // Bitwise update (no table needed) using reflected polynomial 0xEDB88320
  POLY = 3988292384 // 0xEDB88320

  // Compute CRC32 over byte array, return u32 value (bitwise, reflected)
  fun crc32_bytes_value(this, bytes)
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

  // Public: compute CRC32 of hex string of bytes, return 8-char hex
  fun crc32_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    v = this.crc32_bytes_value(bytes)
    return this.u32_to_hex8(v)

  // Convenience: compute CRC32 of ASCII string
  fun crc32_str(this, str)
    bytes = string_to_bytes_ascii(str)
    v = this.crc32_bytes_value(bytes)
    return this.u32_to_hex8(v)
