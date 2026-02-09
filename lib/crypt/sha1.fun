/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

// lib/crypt/sha1.fun
// Pure Fun SHA-1 implementation operating on hex-string or ASCII inputs.
//
// Public API (class):
//   sha = SHA1()
//   sha.sha1_hex(hexStr) -> digest hex string (lowercase)
//   sha.sha1_str("abc")  -> digest hex string of ASCII bytes

#include <hex.fun>
#include <strings.fun>

class SHA1()
  // 32-bit helpers
  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  fun add32(this, a, b)
    return this.u32(a + b)

  fun rol32(this, x, s)
    x = this.u32(x)
    return rol(x, s)

  // hex helpers
  fun byte_from_hex_pair(this, hh)
    hi = hex_val(substr(hh, 0, 1))
    lo = hex_val(substr(hh, 1, 1))
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

  fun bytes_to_hex(this, arr)
    i = 0
    out = []
    while i < len(arr)
      push(out, two_hex(arr[i]))
      i = i + 1
    return join(out, "")

  // padding (SHA-1): append 0x80, then zeros until length ≡ 56 (mod 64), then 64-bit length big-endian
  fun pad_bytes(this, bytes)
    L = len(bytes)
    out = []
    i = 0
    while i < L
      push(out, bytes[i])
      i = i + 1
    push(out, 128)
    while (len(out) % 64) != 56
      push(out, 0)
    len_bits = L * 8
    j = 7
    while j >= 0
      b = (len_bits / pow(2, 8 * (7 - j))) % 256
      push(out, b)
      j = j - 1
    return out

  fun word32_be(this, b0, b1, b2, b3)
    return this.u32(b0 * 16777216 + b1 * 65536 + b2 * 256 + b3)

  // process 512-bit block
  fun process_block(this, state, block)
    // W[80]
    W = []
    t = 0
    while t < 16
      j = t * 4
      push(W, this.word32_be(block[j], block[j+1], block[j+2], block[j+3]))
      t = t + 1
    while t < 80
      wt = bxor(bxor(bxor(W[t-3], W[t-8]), W[t-14]), W[t-16])
      wt = this.rol32(wt, 1)
      push(W, wt)
      t = t + 1

    a = state[0]
    b = state[1]
    c = state[2]
    d = state[3]
    e = state[4]

    t = 0
    while t < 80
      if (t < 20)
        f = bor(band(b, c), band(bnot(b), d))
        k = 1518500249          // 0x5A827999
      else if (t < 40)
        f = bxor(b, bxor(c, d))
        k = 1859775393          // 0x6ED9EBA1
      else if (t < 60)
        f = bor(bor(band(b, c), band(b, d)), band(c, d))
        k = 2400959708          // 0x8F1BBCDC
      else
        f = bxor(b, bxor(c, d))
        k = 3395469782          // 0xCA62C1D6

      temp = this.u32(this.u32(this.rol32(a, 5) + f) + this.u32(this.u32(e + k) + W[t]))
      e = d
      d = c
      c = this.rol32(b, 30)
      b = a
      a = temp
      t = t + 1

    state[0] = this.add32(state[0], a)
    state[1] = this.add32(state[1], b)
    state[2] = this.add32(state[2], c)
    state[3] = this.add32(state[3], d)
    state[4] = this.add32(state[4], e)
    return state

  fun sha1_bytes(this, bytes)
    // initial H
    H = [ 1732584193, 4023233417, 2562383102, 271733878, 3285377520 ]
    data = this.pad_bytes(bytes)
    N = len(data)
    off = 0
    while off < N
      block = []
      i = 0
      while i < 64
        push(block, data[off + i])
        i = i + 1
      H = this.process_block(H, block)
      off = off + 64

    // output 20-byte big-endian
    out = []
    j = 0
    while j < 5
      v = H[j]
      push(out, (v / 16777216) % 256)
      push(out, (v / 65536) % 256)
      push(out, (v / 256) % 256)
      push(out, v % 256)
      j = j + 1
    return out

  fun sha1_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    digest = this.sha1_bytes(bytes)
    return this.bytes_to_hex(digest)

  fun sha1_str(this, str)
    bytes = string_to_bytes_ascii(str)
    digest = this.sha1_bytes(bytes)
    return this.bytes_to_hex(digest)
