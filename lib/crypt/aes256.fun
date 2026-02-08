/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-04
 */

/*
 * AES-256 (ECB) encryption in pure Fun.
 *
 * Public API (class AES256):
 *   encrypt_block_hex(pt_hex32, key_hex64) -> ct_hex32
 *   encrypt_ecb_hex(hexStr, key_hex64) -> ct_hex (hexStr length must be multiple of 32)
 *
 * Example test vector (AES-256, FIPS-197):
 *   key: 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
 *   pt : 00112233445566778899aabbccddeeff
 *   ct : 8ea2b7ca516745bfeafc49904b496089
 */

#include <hex.fun>
#include <strings.fun>

class AES256()
  // ---------- Hex helpers ----------
  fun byte_from_hex_pair(this, hh)
    hi = hex_val(substr(hh, 0, 1))
    lo = hex_val(substr(hh, 1, 1))
    return hi * 16 + lo

  fun from_hex(this, hex)
    arr = []
    i = 0
    n = len(hex)
    while i + 1 < n
      push(arr, this.byte_from_hex_pair(substr(hex, i, 2)))
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

  // ---------- Finite field helpers (GF(2^8)) ----------
  fun b8(this, x)
    // clamp to 0..255
    while x < 0
      x = x + 256
    while x >= 256
      x = x - 256
    return x

  fun xtime(this, x)
    x = this.b8(x)
    hi = band(x, 128)
    x2 = this.b8(shl(x, 1))
    if (hi != 0)
      x2 = bxor(x2, 27)
    return this.b8(x2)

  fun mul2(this, x)
    return this.xtime(x)
  fun mul3(this, x)
    return bxor(this.xtime(x), this.b8(x))

  // ---------- S-box & Rcon ----------
  S = [
    99,124,119,123,242,107,111,197,48,1,103,43,254,215,171,118,
    202,130,201,125,250,89,71,240,173,212,162,175,156,164,114,192,
    183,253,147,38,54,63,247,204,52,165,229,241,113,216,49,21,
    4,199,35,195,24,150,5,154,7,18,128,226,235,39,178,117,
    9,131,44,26,27,110,90,160,82,59,214,179,41,227,47,132,
    83,209,0,237,32,252,177,91,106,203,190,57,74,76,88,207,
    208,239,170,251,67,77,51,133,69,249,2,127,80,60,159,168,
    81,163,64,143,146,157,56,245,188,182,218,33,16,255,243,210,
    205,12,19,236,95,151,68,23,196,167,126,61,100,93,25,115,
    96,129,79,220,34,42,144,136,70,238,184,20,222,94,11,219,
    224,50,58,10,73,6,36,92,194,211,172,98,145,149,228,121,
    231,200,55,109,141,213,78,169,108,86,244,234,101,122,174,8,
    186,120,37,46,28,166,180,198,232,221,116,31,75,189,139,138,
    112,62,181,102,72,3,246,14,97,53,87,185,134,193,29,158,
    225,248,152,17,105,217,142,148,155,30,135,233,206,85,40,223,
    140,161,137,13,191,230,66,104,65,153,45,15,176,84,187,22
  ]

  Rcon = [
    0,
    1,2,4,8,16,32,64,128,27,54,
    108,216,171,77,154
  ]

  // ---------- Key schedule for AES-256 ----------
  // Input key (32 bytes). Output round keys as 240 bytes (Nr=14, Nb=4 -> (Nr+1)*Nb*4 = 60*4).
  fun sub_word(this, w)
    // w is 4-byte array
    return [ this.S[w[0]], this.S[w[1]], this.S[w[2]], this.S[w[3]] ]

  fun rot_word(this, w)
    return [ w[1], w[2], w[3], w[0] ]

  fun key_expansion(this, key_bytes)
    Nk = 8
    Nb = 4
    Nr = 14
    W = [] // words; each word is 4 bytes stored back-to-back in a flat array of bytes

    // copy initial key (8 words -> 32 bytes)
    i = 0
    while i < 32
      push(W, key_bytes[i])
      i = i + 1

    // Expand to 60 words (240 bytes)
    // We work in 4-byte steps, keeping temporary word t (4 bytes)
    bytes_len = 32
    while bytes_len < 240
      // fetch previous word
      t0 = W[bytes_len - 4]
      t1 = W[bytes_len - 3]
      t2 = W[bytes_len - 2]
      t3 = W[bytes_len - 1]
      t = [t0,t1,t2,t3]

      i_words = bytes_len / 4 // word index
      if (i_words % Nk == 0)
        t = this.sub_word(this.rot_word(t))
        rc = this.Rcon[i_words / Nk]
        t[0] = bxor(t[0], rc)
      else if (i_words % Nk == 4)
        t = this.sub_word(t)

      // W[i] = W[i - Nk] xor t
      j = 0
      while j < 4
        prev = W[bytes_len - 32 + j] // 32 bytes == Nk*4
        push(W, bxor(prev, t[j]))
        j = j + 1
      bytes_len = bytes_len + 4
    return W // 240 bytes

  // ---------- State transforms ----------
  fun add_round_key(this, state, roundKeys, round)
    // state: 16 bytes, roundKeys: 240 bytes, round idx 0..14
    // Nb=4, 16 bytes per round
    offset = round * 16
    i = 0
    while i < 16
      state[i] = bxor(state[i], roundKeys[offset + i])
      i = i + 1
    return state

  fun sub_bytes(this, state)
    i = 0
    while i < 16
      state[i] = this.S[state[i]]
      i = i + 1
    return state

  fun shift_rows(this, s)
    // s is 1D [r + 4*c] (row-major within column-major indexing): r in [0..3], c in [0..3]
    // Row 0: no shift
    // Row 1: left by 1
    t = s[1]
    s[1] = s[5]
    s[5] = s[9]
    s[9] = s[13]
    s[13] = t
    // Row 2: left by 2
    t0 = s[2]
    t1 = s[6]
    s[2] = s[10]
    s[6] = s[14]
    s[10] = t0
    s[14] = t1
    // Row 3: left by 3 (aka right by 1)
    t = s[15]
    s[15] = s[11]
    s[11] = s[7]
    s[7] = s[3]
    s[3] = t
    return s

  fun mix_single_column(this, a0, a1, a2, a3)
    // returns [r0,r1,r2,r3]
    r0 = bxor(bxor(bxor(this.mul2(a0), this.mul3(a1)), a2), a3)
    r1 = bxor(bxor(bxor(a0, this.mul2(a1)), this.mul3(a2)), a3)
    r2 = bxor(bxor(bxor(a0, a1), this.mul2(a2)), this.mul3(a3))
    r3 = bxor(bxor(bxor(this.mul3(a0), a1), a2), this.mul2(a3))
    return [this.b8(r0), this.b8(r1), this.b8(r2), this.b8(r3)]

  fun mix_columns(this, s)
    c = 0
    while c < 4
      i = c * 4
      col = this.mix_single_column(s[i], s[i+1], s[i+2], s[i+3])
      s[i] = col[0]
      s[i+1] = col[1]
      s[i+2] = col[2]
      s[i+3] = col[3]
      c = c + 1
    return s

  // ---------- Block encryption ----------
  fun encrypt_block_bytes(this, pt16, key32)
    // pt16: 16 bytes array, key32: 32 bytes array
    // returns 16 bytes array (ciphertext)
    Nb = 4
    Nr = 14
    rk = this.key_expansion(key32) // 240 bytes

    // copy state
    s = []
    i = 0
    while i < 16
      push(s, pt16[i])
      i = i + 1

    // round 0
    s = this.add_round_key(s, rk, 0)

    round = 1
    while round < Nr
      s = this.sub_bytes(s)
      s = this.shift_rows(s)
      s = this.mix_columns(s)
      s = this.add_round_key(s, rk, round)
      round = round + 1

    // final round (no MixColumns)
    s = this.sub_bytes(s)
    s = this.shift_rows(s)
    s = this.add_round_key(s, rk, Nr)
    return s

  fun encrypt_block_hex(this, pt_hex32, key_hex64)
    // Validate lengths
    if (len(pt_hex32) != 32)
      // Return empty on invalid input to match common style
      return ""
    if (len(key_hex64) != 64)
      return ""
    pt = this.from_hex(pt_hex32)
    key = this.from_hex(key_hex64)
    ct = this.encrypt_block_bytes(pt, key)
    return this.bytes_to_hex(ct)

  fun encrypt_ecb_hex(this, hexStr, key_hex64)
    // Robust handling that avoids substr semantics ambiguity by working on bytes
    // Validate key
    if (len(key_hex64) != 64)
      return ""
    // Validate hexStr length: must be even and represent a whole number of 16-byte blocks
    if ((len(hexStr) % 2) != 0)
      return ""
    bytes = this.from_hex(hexStr)
    if ((len(bytes) % 16) != 0)
      return ""
    key = this.from_hex(key_hex64)
    out_bytes = []
    off = 0
    total = len(bytes)
    while off < total
      // collect 16-byte block
      blk = []
      j = 0
      while j < 16
        push(blk, bytes[off + j])
        j = j + 1
      ct_blk = this.encrypt_block_bytes(blk, key)
      // append to output
      k = 0
      while k < 16
        push(out_bytes, ct_blk[k])
        k = k + 1
      off = off + 16
    return this.bytes_to_hex(out_bytes)
