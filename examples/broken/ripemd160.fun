/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-02
 */

// lib/crypt/ripemd160.fun
// Pure Fun implementation of RIPEMD-160 operating on hex-string or ASCII inputs.
//
// Public API (class):
//   rip = RIPEMD160()
//   rip.ripemd160_hex(hexStr) -> digest hex string (lowercase)
//   rip.ripemd160_str("abc")  -> digest hex string of ASCII bytes

#include <strings.fun>

class RIPEMD160()
  // 32-bit helpers (match style from md5.fun)
  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  fun add32(this, a, b)
    return this.u32(a + b)

  fun add32_5(this, a, b, c, d, e)
    return this.u32(this.u32(this.u32(this.u32(a + b) + c) + d) + e)

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

  // hex helpers (same as md5/sha files)
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

  // Padding (RIPEMD-160): like MD4/MD5 — append 0x80, zeros to 56 mod 64, then 64-bit length in little-endian
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

  // RIPEMD-160 boolean functions
  fun f1(this, x, y, z)
    return this.xor32(this.xor32(x, y), z)

  fun f2(this, x, y, z)
    return this.or32(this.and32(x, y), this.and32(this.not32(x), z))

  fun f3(this, x, y, z)
    return this.xor32(this.or32(x, this.not32(y)), z)

  fun f4(this, x, y, z)
    return this.or32(this.and32(x, z), this.and32(y, this.not32(z)))

  fun f5(this, x, y, z)
    return this.xor32(x, this.or32(y, this.not32(z)))

  // Process a 512-bit block
  fun process_block(this, H, block)
    // message words X[16] (little-endian)
    X = []
    i = 0
    while i < 16
      j = i * 4
      push(X, this.word32_le(block[j], block[j+1], block[j+2], block[j+3]))
      i = i + 1

    // R and S (left line)
    R = [
      0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
      7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,
      3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,
      1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,
      4,0,5,9,7,12,2,10,14,1,3,8,11,6,15,13
    ]
    S = [
      11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,
      7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,
      11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,
      11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,
      9,15,5,11,6,8,13,12,5,12,13,14,11,8,5,6
    ]

    // R' and S' (right line)
    Rp = [
      5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12,
      6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2,
      15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13,
      8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14,
      12,15,10,4,1,5,8,7,6,2,13,14,0,3,9,11
    ]
    Sp = [
      8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6,
      9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11,
      9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5,
      15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8,
      8,5,12,9,12,5,14,6,8,13,6,5,15,13,11,11
    ]

    // Constants
    KL = [0, 1518500249, 1859775393, 2400959708, 2840853838]       // 0x00, 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xA953FD4E
    KR = [1352829926, 1548603684, 1836072691, 2053994217, 0]       // 0x50A28BE6, 0x5C4DD124, 0x6D703EF3, 0x7A6D76E9, 0x00000000

    al = H[0]; bl = H[1]; cl = H[2]; dl = H[3]; el = H[4]
    ar = H[0]; br = H[1]; cr = H[2]; dr = H[3]; er = H[4]

    j = 0
    while j < 80
      sL = S[j]
      sR = Sp[j]

      // select function and constant per round for left lane
      if (j < 16)
        fL = this.f1(bl, cl, dl)
        kL = KL[0]
      else if (j < 32)
        fL = this.f2(bl, cl, dl)
        kL = KL[1]
      else if (j < 48)
        fL = this.f3(bl, cl, dl)
        kL = KL[2]
      else if (j < 64)
        fL = this.f4(bl, cl, dl)
        kL = KL[3]
      else
        fL = this.f5(bl, cl, dl)
        kL = KL[4]

      // right lane
      if (j < 16)
        fR = this.f5(br, cr, dr)
        kR = KR[0]
      else if (j < 32)
        fR = this.f4(br, cr, dr)
        kR = KR[1]
      else if (j < 48)
        fR = this.f3(br, cr, dr)
        kR = KR[2]
      else if (j < 64)
        fR = this.f2(br, cr, dr)
        kR = KR[3]
      else
        fR = this.f1(br, cr, dr)
        kR = KR[4]

      // left step
      tl = this.add32_5(al, fL, X[R[j]], kL, 0)
      tl = this.rol32(tl, sL)
      tl = this.add32(tl, el)
      al = el
      el = dl
      dl = this.rol32(cl, 10)
      cl = bl
      bl = tl

      // right step
      tr = this.add32_5(ar, fR, X[Rp[j]], kR, 0)
      tr = this.rol32(tr, sR)
      tr = this.add32(tr, er)
      ar = er
      er = dr
      dr = this.rol32(cr, 10)
      cr = br
      br = tr

      j = j + 1

    // combine using originals
    h0 = H[0]; h1 = H[1]; h2 = H[2]; h3 = H[3]; h4 = H[4]
    tt = this.add32(h0, this.add32(bl, cr))
    H[0] = this.add32(h1, this.add32(cl, dr))
    H[1] = this.add32(h2, this.add32(dl, er))
    H[2] = this.add32(h3, this.add32(el, ar))
    H[3] = this.add32(h4, this.add32(al, br))
    H[4] = tt
    return H

  fun ripemd160_bytes(this, bytes)
    // initialize h0..h4
    H = [1732584193, 4023233417, 2562383102, 271733878, 3285377520]
    data = this.pad_bytes(bytes)
    off = 0
    N = len(data)
    while off < N
      block = []
      i = 0
      while i < 64
        push(block, data[off + i])
        i = i + 1
      H = this.process_block(H, block)
      off = off + 64

    // output as little-endian of h0..h4 (20 bytes)
    out = []
    j = 0
    while j < 5
      v = H[j]
      push(out, v % 256)
      push(out, (v / 256) % 256)
      push(out, (v / 65536) % 256)
      push(out, (v / 16777216) % 256)
      j = j + 1
    return out

  fun ripemd160_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    digest = this.ripemd160_bytes(bytes)
    return this.bytes_to_hex(digest)

  fun ripemd160_str(this, str)
    bytes = string_to_bytes_ascii(str)
    digest = this.ripemd160_bytes(bytes)
    return this.bytes_to_hex(digest)
