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

// lib/crypt/sha512.fun
// Pure Fun SHA-512 implementation operating on hex-string input.
// 64-bit words are represented as [hi, lo] (two uint32 parts).
//
// Public API (class):
//   s = SHA512()
//   s.sha512_hex(hexStr) -> digest hex string (lowercase)
//
// Example:
//   print(SHA512().sha512_hex("616263"))  // "abc" =>
//   ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
//   8: that’s for SHA-256; SHA-512 for "abc" is:
//   ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a
//   2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f

#include <hex.fun>
#include <strings.fun>

class SHA512()
  // -------- hex helpers --------
  fun byte_from_hex_pair(this, hh)
    number hi = hex_val(substr(hh, 0, 1))
    number lo = hex_val(substr(hh, 1, 1))
    return hi * 16 + lo

  fun from_hex(this, hex)
    hex = to_string(hex)
    arr = []
    number i = 0
    number n = len(hex)
    while i + 1 < n
      number b = this.byte_from_hex_pair(substr(hex, i, 2))
      push(arr, b)
      i = i + 2
    return arr

  fun bytes_to_hex(this, arr)
    out = []
    number i = 0
    number L = len(arr)
    while i < L
      push(out, two_hex(arr[i]))
      i = i + 1
    return join(out, "")

  // -------- 32/64 helpers --------
  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  // 64-bit as [hi, lo] (each uint32)
  fun pack64_be(this, b0,b1,b2,b3,b4,b5,b6,b7)
    number hi = this.u32(b0 * 16777216 + b1 * 65536 + b2 * 256 + b3)
    number lo = this.u32(b4 * 16777216 + b5 * 65536 + b6 * 256 + b7)
    return [hi, lo]

  fun add64(this, a, b)
    number alo = this.u32(a[1])
    number ahi = this.u32(a[0])
    number blo = this.u32(b[1])
    number bhi = this.u32(b[0])
    number sum_lo = alo + blo
    number carry = sum_lo / 4294967296
    sum_lo = sum_lo % 4294967296
    number sum_hi = (ahi + bhi + carry) % 4294967296
    return [sum_hi, sum_lo]

  fun add64_5(this, a, b, c, d, e)
    return this.add64(this.add64(this.add64(this.add64(a, b), c), d), e)

  fun xor64(this, a, b)
    return [ bxor(a[0], b[0]), bxor(a[1], b[1]) ]

  fun and64(this, a, b)
    return [ band(a[0], b[0]), band(a[1], b[1]) ]

  fun or64(this, a, b)
    return [ bor(a[0], b[0]), bor(a[1], b[1]) ]

  fun not64(this, a)
    return [ bnot(a[0]), bnot(a[1]) ]

  // logical right shift by n (0..63)
  fun shr64(this, a, n)
    number s = n % 64
    number hi = a[0]
    number lo = a[1]
    if (s == 0)
      return [hi, lo]
    else if (s < 32)
      number new_lo = bor( shr(lo, s), shl(hi, 32 - s) )
      number new_hi = shr(hi, s)
      return [ new_hi, new_lo ]
    else
      number k = s - 32
      number new_lo2 = shr(hi, k)
      return [ 0, new_lo2 ]

  // rotate right by n (0..63)
  fun rotr64(this, a, n)
    number s = n % 64
    number hi = a[0]
    number lo = a[1]
    if (s == 0)
      return [hi, lo]
    else if (s < 32)
      number new_lo = bor( shr(lo, s), shl(hi, 32 - s) )
      number new_hi = bor( shr(hi, s), shl(lo, 32 - s) )
      return [ new_hi, new_lo ]
    else
      number k = s - 32
      // swap roles
      number new_lo2 = bor( shr(hi, k), shl(lo, 32 - k) )
      number new_hi2 = bor( shr(lo, k), shl(hi, 32 - k) )
      return [ new_hi2, new_lo2 ]

  // SHA-512 logical functions
  fun Ch(this, x, y, z)
    // (x & y) ^ (~x & z)
    return this.xor64(this.and64(x, y), this.and64(this.not64(x), z))

  fun Maj(this, x, y, z)
    // (x & y) ^ (x & z) ^ (y & z)
    return this.xor64(this.xor64(this.and64(x, y), this.and64(x, z)), this.and64(y, z))

  fun Sigma0(this, x)
    return this.xor64(this.xor64(this.rotr64(x, 28), this.rotr64(x, 34)), this.rotr64(x, 39))

  fun Sigma1(this, x)
    return this.xor64(this.xor64(this.rotr64(x, 14), this.rotr64(x, 18)), this.rotr64(x, 41))

  fun sigma0(this, x)
    return this.xor64(this.xor64(this.rotr64(x, 1), this.rotr64(x, 8)), this.shr64(x, 7))

  fun sigma1(this, x)
    return this.xor64(this.xor64(this.rotr64(x, 19), this.rotr64(x, 61)), this.shr64(x, 6))

  // -------- constants --------
  IV = [
    [0x6a09e667, 0xf3bcc908], [0xbb67ae85, 0x84caa73b], [0x3c6ef372, 0xfe94f82b], [0xa54ff53a, 0x5f1d36f1],
    [0x510e527f, 0xade682d1], [0x9b05688c, 0x2b3e6c1f], [0x1f83d9ab, 0xfb41bd6b], [0x5be0cd19, 0x137e2179]
  ]

  K = [
    [0x428a2f98,0xd728ae22],[0x71374491,0x23ef65cd],[0xb5c0fbcf,0xec4d3b2f],[0xe9b5dba5,0x8189dbbc],
    [0x3956c25b,0xf348b538],[0x59f111f1,0xb605d019],[0x923f82a4,0xaf194f9b],[0xab1c5ed5,0xda6d8118],
    [0xd807aa98,0xa3030242],[0x12835b01,0x45706fbe],[0x243185be,0x4ee4b28c],[0x550c7dc3,0xd5ffb4e2],
    [0x72be5d74,0xf27b896f],[0x80deb1fe,0x3b1696b1],[0x9bdc06a7,0x25c71235],[0xc19bf174,0xcf692694],
    [0xe49b69c1,0x9ef14ad2],[0xefbe4786,0x384f25e3],[0x0fc19dc6,0x8b8cd5b5],[0x240ca1cc,0x77ac9c65],
    [0x2de92c6f,0x592b0275],[0x4a7484aa,0x6ea6e483],[0x5cb0a9dc,0xbd41fbd4],[0x76f988da,0x831153b5],
    [0x983e5152,0xee66dfab],[0xa831c66d,0x2db43210],[0xb00327c8,0x98fb213f],[0xbf597fc7,0xbeef0ee4],
    [0xc6e00bf3,0x3da88fc2],[0xd5a79147,0x930aa725],[0x06ca6351,0xe003826f],[0x14292967,0x0a0e6e70],
    [0x27b70a85,0x46d22ffc],[0x2e1b2138,0x5c26c926],[0x4d2c6dfc,0x5ac42aed],[0x53380d13,0x9d95b3df],
    [0x650a7354,0x8baf63de],[0x766a0abb,0x3c77b2a8],[0x81c2c92e,0x47edaee6],[0x92722c85,0x1482353b],
    [0xa2bfe8a1,0x4cf10364],[0xa81a664b,0xbc423001],[0xc24b8b70,0xd0f89791],[0xc76c51a3,0x0654be30],
    [0xd192e819,0xd6ef5218],[0xd6990624,0x5565a910],[0xf40e3585,0x5771202a],[0x106aa070,0x32bbd1b8],
    [0x19a4c116,0xb8d2d0c8],[0x1e376c08,0x5141ab53],[0x2748774c,0xdf8eeb99],[0x34b0bcb5,0xe19b48a8],
    [0x391c0cb3,0xc5c95a63],[0x4ed8aa4a,0xe3418acb],[0x5b9cca4f,0x7763e373],[0x682e6ff3,0xd6b2b8a3],
    [0x748f82ee,0x5defb2fc],[0x78a5636f,0x43172f60],[0x84c87814,0xa1f0ab72],[0x8cc70208,0x1a6439ec],
    [0x90befffa,0x23631e28],[0xa4506ceb,0xde82bde9],[0xbef9a3f7,0xb2c67915],[0xc67178f2,0xe372532b],
    [0xca273ece,0xea26619c],[0xd186b8c7,0x21c0c207],[0xeada7dd6,0xcde0eb1e],[0xf57d4f7f,0xee6ed178],
    [0x06f067aa,0x72176fba],[0x0a637dc5,0xa2c898a6],[0x113f9804,0xbef90dae],[0x1b710b35,0x131c471b],
    [0x28db77f5,0x23047d84],[0x32caab7b,0x40c72493],[0x3c9ebe0a,0x15c9bebc],[0x431d67c4,0x9c100d4c],
    [0x4cc5d4be,0xcb3e42b6],[0x597f299c,0xfc657e2a],[0x5fcb6fab,0x3ad6faec],[0x6c44198c,0x4a475817]
  ]

  // -------- padding --------
  fun pad_bytes(this, bytes)
    number L = len(bytes)
    out = []
    // copy bytes
    number i = 0
    while i < L
      push(out, bytes[i])
      i = i + 1
    // append 0x80
    push(out, 128)
    // pad zeros until length % 128 == 112 (i.e., 896 bits)
    while (len(out) % 128) != 112
      push(out, 0)
    // append 128-bit length (we use upper 64 bits zero, lower 64 = L*8)
    // upper 8 bytes zero:
    number u = 0
    number j = 0
    while j < 8
      push(out, 0)
      j = j + 1
    // lower 8 bytes big-endian (most significant byte first)
    number bits = L * 8
    number t = 7
    while t >= 0
      number b = (bits / pow(2, 8 * t)) % 256
      push(out, b)
      t = t - 1
    return out

  // -------- core --------
  fun process_block(this, H, block)
    // W ring buffer of 16 words ([hi, lo])
    W = []
    number i = 0
    while i < 16
      number j = i * 8
      push(W, this.pack64_be(block[j],block[j+1],block[j+2],block[j+3],
                             block[j+4],block[j+5],block[j+6],block[j+7]))
      i = i + 1

    a = H[0]
    b = H[1]
    c = H[2]
    d = H[3]
    e = H[4]
    f = H[5]
    g = H[6]
    h = H[7]

    number t = 0
    while t < 80
      number idx = t % 16
      if (t >= 16)
        w2  = W[(t - 2) % 16]
        w7  = W[(t - 7) % 16]
        w15 = W[(t - 15) % 16]
        w16 = W[(t - 16) % 16]  // current slot value
        s1  = this.sigma1(w2)
        s0  = this.sigma0(w15)
        tmp = this.add64(w16, s0)
        tmp = this.add64(tmp, w7)
        W[idx] = this.add64(tmp, s1)
      Wt = W[idx]

      // Precompute hot terms once per round
      Se1 = this.Sigma1(e)
      ch  = this.Ch(e, f, g)

      // T1 = h + Se1 + ch + K[t] + Wt (chained to avoid 5-arg helper)
      tmp1 = this.add64(h, Se1)
      tmp1 = this.add64(tmp1, ch)
      tmp1 = this.add64(tmp1, this.K[t])
      T1   = this.add64(tmp1, Wt)

      // T2 = Sigma0(a) + Maj(a, b, c)
      T2 = this.add64(this.Sigma0(a), this.Maj(a, b, c))

      // State update
      h  = g
      g  = f
      f  = e
      e  = this.add64(d, T1)
      d  = c
      c  = b
      b  = a
      a  = this.add64(T1, T2)
      t  = t + 1

    H[0] = this.add64(H[0], a)
    H[1] = this.add64(H[1], b)
    H[2] = this.add64(H[2], c)
    H[3] = this.add64(H[3], d)
    H[4] = this.add64(H[4], e)
    H[5] = this.add64(H[5], f)
    H[6] = this.add64(H[6], g)
    H[7] = this.add64(H[7], h)
    return H

  fun sha512_bytes(this, bytes)
    data = this.pad_bytes(bytes)
    // init H (deep copy IV)
    H = [ this.IV[0], this.IV[1], this.IV[2], this.IV[3], this.IV[4], this.IV[5], this.IV[6], this.IV[7] ]
    number off = 0
    number N = len(data)
    while off < N
      block = []
      number i = 0
      while i < 128
        push(block, data[off + i])
        i = i + 1
      H = this.process_block(H, block)
      off = off + 128

    // output 64 bytes big-endian from H[0..7]
    out = []
    number j = 0
    while j < 8
      word = H[j]
      number hi = word[0]
      number lo = word[1]
      // hi
      push(out, (hi / 16777216) % 256)
      push(out, (hi / 65536) % 256)
      push(out, (hi / 256) % 256)
      push(out, hi % 256)
      // lo
      push(out, (lo / 16777216) % 256)
      push(out, (lo / 65536) % 256)
      push(out, (lo / 256) % 256)
      push(out, lo % 256)
      j = j + 1
    return out

  fun sha512_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    digest = this.sha512_bytes(bytes)
    return this.bytes_to_hex(digest)

  // Hash raw string bytes (printable ASCII). Example: sha512_str("616263") -> ee2109...
  fun sha512_str(this, str)
    bytes = string_to_bytes_ascii(str)
    digest = this.sha512_bytes(bytes)
    return this.bytes_to_hex(digest)
