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

// lib/crypt/sha256.fun
// Pure Fun SHA-256 implementation operating on hex-string input.
//
// Public API (class):
//   sha = SHA256()
//   sha.sha256_hex(hexStr) -> digest hex string (lowercase)
//
// Example:
//   print(SHA256().sha256_hex("616263"))  // "abc" => ba7816bf...

#include <hex.fun>
#include <strings.fun>

class SHA256()
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

  fun add32_5(this, a, b, c, d, e)
    return this.u32(this.u32(this.u32(this.u32(a + b) + c) + d) + e)

  // bitwise (use native VM ops)
  fun rotr32(this, x, s)
    return ror(this.u32(x), s)

  fun shr32(this, x, s)
    return shr(this.u32(x), s)

  fun ch(this, x, y, z)
    // (x & y) ^ (~x & z)
    return bxor(band(x, y), band(bnot(x), z))

  fun maj(this, x, y, z)
    // (x & y) ^ (x & z) ^ (y & z)
    return bxor(bxor(band(x, y), band(x, z)), band(y, z))

  fun bigSigma0(this, x)
    // ROTR2 ^ ROTR13 ^ ROTR22
    return bxor(bxor(this.rotr32(x, 2), this.rotr32(x, 13)), this.rotr32(x, 22))

  fun bigSigma1(this, x)
    // ROTR6 ^ ROTR11 ^ ROTR25
    return bxor(bxor(this.rotr32(x, 6), this.rotr32(x, 11)), this.rotr32(x, 25))

  fun smallSigma0(this, x)
    // ROTR7 ^ ROTR18 ^ SHR3
    return bxor(bxor(this.rotr32(x, 7), this.rotr32(x, 18)), this.shr32(x, 3))

  fun smallSigma1(this, x)
    // ROTR17 ^ ROTR19 ^ SHR10
    return bxor(bxor(this.rotr32(x, 17), this.rotr32(x, 19)), this.shr32(x, 10))

  // constants
  IV = [
    1779033703, 3144134277, 1013904242, 2773480762,
    1359893119, 2600822924, 528734635,  1541459225
  ]

  K = [
    1116352408, 1899447441, 3049323471, 3921009573, 961987163,  1508970993, 2453635748, 2870763221,
    3624381080, 310598401,  607225278,  1426881987, 1925078388, 2162078206, 2614888103, 3248222580,
    3835390401, 4022224774, 264347078,  604807628,  770255983,  1249150122, 1555081692, 1996064986,
    2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993,  338241895,
    666307205,  773529912,  1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037,
    2730485921, 2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344,
    430227734,  506948616,  659060556,  883997877,  958139571,  1322822218, 1537002063, 1747873779,
    1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298
  ]

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

  // padding (SHA-256)
  fun pad_bytes(this, bytes)
    L = len(bytes)
    out = []
    i = 0
    while i < L
      push(out, bytes[i])
      i = i + 1
    // append 0x80
    push(out, 128)
    // zeros until length % 64 == 56
    while (len(out) % 64) != 56
      push(out, 0)
    // length in bits as 64-bit big-endian
    len_bits = L * 8
    j = 7
    while j >= 0
      b = (len_bits / pow(2, 8 * (7 - j))) % 256
      push(out, b)
      j = j - 1
    return out

  fun word32_be(this, b0, b1, b2, b3)
    // big-endian
    return this.u32(b0 * 16777216 + b1 * 65536 + b2 * 256 + b3)

  fun process_block(this, state, block)
    // prepare message schedule W[64]
    W = []
    t = 0
    while t < 16
      j = t * 4
      push(W, this.word32_be(block[j], block[j+1], block[j+2], block[j+3]))
      t = t + 1

    while t < 64
      s0 = this.smallSigma0(W[t - 15])
      s1 = this.smallSigma1(W[t - 2])
      wt = this.add32_5(W[t - 16], s0, W[t - 7], s1, 0)
      // reuse array position via push (we can index existing after push)
      push(W, wt)
      t = t + 1

    a = state[0]
    b = state[1]
    c = state[2]
    d = state[3]
    e = state[4]
    f = state[5]
    g = state[6]
    h = state[7]

    t = 0
    while t < 64
      T1 = this.add32_5(h, this.bigSigma1(e), this.ch(e, f, g), this.K[t], W[t])
      T2 = this.add32(this.bigSigma0(a), this.maj(a, b, c))
      h = g
      g = f
      f = e
      e = this.add32(d, T1)
      d = c
      c = b
      b = a
      a = this.add32(T1, T2)
      t = t + 1

    state[0] = this.add32(state[0], a)
    state[1] = this.add32(state[1], b)
    state[2] = this.add32(state[2], c)
    state[3] = this.add32(state[3], d)
    state[4] = this.add32(state[4], e)
    state[5] = this.add32(state[5], f)
    state[6] = this.add32(state[6], g)
    state[7] = this.add32(state[7], h)
    return state

  fun sha256_bytes(this, bytes)
    data = this.pad_bytes(bytes)
    // initial hash value (copy IV)
    H = [ this.IV[0], this.IV[1], this.IV[2], this.IV[3], this.IV[4], this.IV[5], this.IV[6], this.IV[7] ]

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

    // digest: 32 bytes big-endian
    out = []
    j = 0
    while j < 8
      v = H[j]
      push(out, (v / 16777216) % 256)
      push(out, (v / 65536) % 256)
      push(out, (v / 256) % 256)
      push(out, (v) % 256)
      j = j + 1
    return out

  fun sha256_hex(this, hexStr)
    bytes = this.from_hex(hexStr)
    digest = this.sha256_bytes(bytes)
    return this.bytes_to_hex(digest)

  // Hash raw string bytes (printable ASCII). Example: sha256_str("616263") -> ee2109...
  fun sha256_str(this, str)
    bytes = string_to_bytes_ascii(str)
    digest = this.sha256_bytes(bytes)
    return this.bytes_to_hex(digest)
