/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-29
 */

// lib/crypt/md5.fun
// Pure Fun implementation of MD5 operating on hex-string input.
//
// Public API:
//   md5_hex(hexStr) -> digest hex string (lowercase)
//
// Example:
//   print(md5_hex("616263"))  // "abc" => 900150983cd24fb0d6963f7d28e17f72

// Class wrapper to avoid global name collisions and show class usage

#include <strings.fun>

class MD5()
  // MD5 constants (as fields)
  K = [
    3614090360, 3905402710, 606105819, 3250441966, 4118548399, 1200080426, 2821735955, 4249261313,
    1770035416, 2336552879, 4294925233, 2304563134, 1804603682, 4254626195, 2792965006, 1236535329,
    4129170786, 3225465664, 643717713, 3921069994, 3593408605, 38016083,   3634488961, 3889429448,
    568446438,  3275163606, 4107603335, 1163531501, 2850285829, 4243563512, 1735328473, 2368359562,
    4294588738, 2272392833, 1839030562, 4259657740, 2763975236, 1272893353, 4139469664, 3200236656,
    681279174,  3936430074, 3572445317, 76029189,   3654602809, 3873151461, 530742520,  3299628645,
    4096336452, 1126891415, 2878612391, 4237533241, 1700485571, 2399980690, 4293915773, 2240044497,
    1873313359, 4264355552, 2734768916, 1309151649, 4149444226, 3174756917, 718787259,  3951481745
  ]
  S = [
    7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
    5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
    4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
    6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
  ]

  // 32-bit helpers
  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  fun shl32(this, x, s)
    return shl(this.u32(x), s)

  fun shr32(this, x, s)
    return shr(this.u32(x), s)

  fun rol32(this, x, s)
    return rol(this.u32(x), s)

  fun and32(this, a, b)
    return band(this.u32(a), this.u32(b))

  fun or32(this, a, b)
    return bor(this.u32(a), this.u32(b))

  fun xor32(this, a, b)
    return bxor(this.u32(a), this.u32(b))

  fun not32(this, x)
    return bnot(this.u32(x))

  // tiny adders
  fun add32(this, a, b)
    return this.u32(a + b)

  fun add32_4(this, a, b, c, d)
    return this.u32(this.u32(this.u32(a + b) + c) + d)

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

  // MD5 aux
  fun F(this, x, y, z)
    return this.or32(this.and32(x, y), this.and32(this.not32(x), z))

  fun G(this, x, y, z)
    return this.or32(this.and32(x, z), this.and32(y, this.not32(z)))

  fun H(this, x, y, z)
    return this.xor32(this.xor32(x, y), z)

  fun I(this, x, y, z)
    return this.xor32(y, this.or32(x, this.not32(z)))

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
    j = 0
    while j < 8
      b = (len_bits / pow(2, 8 * j)) % 256
      push(out, b)
      j = j + 1
    return out

  fun word32_le(this, b0, b1, b2, b3)
    return this.u32(b0 + b1 * 256 + b2 * 65536 + b3 * 16777216)

  fun process_block(this, state, block)
    a0 = state[0]
    b0 = state[1]
    c0 = state[2]
    d0 = state[3]

    M = []
    i = 0
    while i < 16
      j = i * 4
      w = this.word32_le(block[j], block[j+1], block[j+2], block[j+3])
      push(M, w)
      i = i + 1

    A = a0
    B = b0
    C = c0
    D = d0

    k = 0
    while k < 64
      if (k < 16)
        f = this.F(B, C, D)
        g = k
      else if (k < 32)
        f = this.G(B, C, D)
        g = (5 * k + 1) % 16
      else if (k < 48)
        f = this.H(B, C, D)
        g = (3 * k + 5) % 16
      else
        f = this.I(B, C, D)
        g = (7 * k) % 16

      tmp = D
      sum = this.add32_4(A, f, this.K[k], M[g])
      D = C
      C = B
      B = this.add32(B, this.rol32(sum, this.S[k]))
      A = tmp
      k = k + 1

    state[0] = this.add32(state[0], A)
    state[1] = this.add32(state[1], B)
    state[2] = this.add32(state[2], C)
    state[3] = this.add32(state[3], D)
    return state

  fun md5_bytes(this, bytes)
    data = this.pad_bytes(bytes)
    a0 = 1732584193
    b0 = 4023233417
    c0 = 2562383102
    d0 = 271733878
    state = [a0, b0, c0, d0]

    off = 0
    N = len(data)
    while off < N
      block = []
      i = 0
      while i < 64
        push(block, data[off + i])
        i = i + 1
      state = this.process_block(state, block)
      off = off + 64

    out = []
    j = 0
    while j < 4
      v = state[j]
      push(out, (v) % 256)
      push(out, (v / 256) % 256)
      push(out, (v / 65536) % 256)
      push(out, (v / 16777216) % 256)
      j = j + 1
    return out

  fun md5_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    digest = this.md5_bytes(bytes)
    return this.bytes_to_hex(digest)

  fun md5_str(this, str)
    bytes = string_to_bytes_ascii(str)
    digest = this.md5_bytes(bytes)
    return this.bytes_to_hex(digest)
