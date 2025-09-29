/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 *
 * Added: 2025-09-29
 */

// lib/crypt/crc32.fun
// Pure Fun CRC32 (IEEE 802.3, polynomial 0xEDB88320, reflected) operating on hex-string input.
//
// Public API (class):
//   crc = CRC32()
//   crc.crc32_hex(hexStr) -> digest hex string (lowercase, 8 hex chars)
//
// Example:
//   print(CRC32().crc32_hex("616263"))  // "abc" => 352441c2

class CRC32()
  // hex helpers
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
    // ensure we operate on a string
    hex = to_string(hex)
    arr = []
    number i = 0
    while true
      ch1 = substr(hex, i, 1)
      if (typeof(ch1) != "String")
        break
      if (ch1 == "")
        break
      ch2 = substr(hex, i + 1, 1)
      if (typeof(ch2) != "String")
        break
      if (ch2 == "")
        break
      pair = join([ch1, ch2], "")
      number b = this.byte_from_hex_pair(pair)
      push(arr, b)
      i = i + 2
    return arr

  fun two_hex(this, n)
    n = n % 256
    hexd = "0123456789abcdef"
    number hi = n / 16
    number lo = n % 16
    ch1 = substr(hexd, hi, 1)
    ch2 = substr(hexd, lo, 1)
    return join([ch1, ch2], "")

  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  // Core per-byte CRC (bit-by-bit, reflected), polynomial 0xEDB88320
  fun crc32_bytes(this, bytes)
    crc = 4294967295       // 0xFFFFFFFF initial
    poly = 3988292384      // 0xEDB88320
      number i = 0
    L = len(bytes)
    while i < L
      // crc ^= byte
      crc = bxor(crc, bytes[i])
      // 8 times
      j = 0
      while j < 8
        if (band(crc, 1) == 1)
          // crc = (crc >> 1) ^ poly
          crc = bxor(shr(crc, 1), poly)
        else
          // crc >>= 1
          crc = shr(crc, 1)
        j = j + 1
      i = i + 1
    // final xor
    crc = bxor(crc, 4294967295)
    return this.u32(crc)

  fun crc32_hex(this, hexStr)
    // Process hex input directly with only core built-ins; no method calls on 'this'
    hex = to_string(hexStr)
    hexd = "0123456789abcdef"

    // CRC state
    number crc = 4294967295       // 0xFFFFFFFF
    number poly = 3988292384      // 0xEDB88320

    // Iterate two hex nibbles at a time
    number i = 0
    while true
      ch1 = substr(hex, i, 1)
      if (typeof(ch1) != "String" || ch1 == "")
        break
      ch2 = substr(hex, i + 1, 1)
      if (typeof(ch2) != "String" || ch2 == "")
        break

      // Convert two hex chars to a byte without helper calls
      // find returns index of substring or -1; ensure lowercase
      h1 = find(hexd, to_string(ch1))
      h2 = find(hexd, to_string(ch2))
      if (h1 < 0 || h2 < 0)
        // invalid hex -> stop
        break
      number b = h1 * 16 + h2

      // crc ^= b
      crc = bxor(crc, b)
      // 8 rounds
      number j = 0
      while j < 8
        if (band(crc, 1) == 1)
          crc = bxor(shr(crc, 1), poly)
        else
          crc = shr(crc, 1)
        j = j + 1

      i = i + 2

    // final xor
    crc = bxor(crc, 4294967295)

    // Format result as 8 hex chars big-endian without calling this.two_hex
    number b3 = (crc / 16777216) % 256
    number b2 = (crc / 65536) % 256
    number b1 = (crc / 256) % 256
    number b0 = crc % 256

    // encode a single byte to two hex chars using hexd and substr
    fun b2hex(byte)
      number hi = (byte / 16) % 16
      number lo = (byte % 16)
      c1 = substr(hexd, hi, 1)
      c2 = substr(hexd, lo, 1)
      return join([c1, c2], "")

    parts = [ b2hex(b3), b2hex(b2), b2hex(b1), b2hex(b0) ]
    return join(parts, "")
