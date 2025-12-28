/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-27
 */

// THIS IS BROKEN ACTUALLY! IT DOES NOT CALCULATE THE EXPECTED HASHES!

#include <strings.fun>

class RIPEMD160()
  KL = [0, 1518500249, 1859775393, 2400959708, 2840853838]
  KR = [1352829926, 1548603684, 1836062451, 2054012649, 0]

  RL = [
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
    3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
    1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
    4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13
  ]

  RR = [
    5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
    6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
    15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
    8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
    12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
  ]

  SL = [
    11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
    7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
    11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
    11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
    9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6
  ]

  SR = [
    8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
    9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
    9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
    15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
    8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11
  ]

  fun u32(this, x)
    m = 4294967296
    while x < 0
      x = x + m
    while x >= m
      x = x - m
    return x

  fun rol32(this, x, s)
    return rol(this.u32(x), s)

  fun add32(this, a, b)
    return this.u32(a + b)

  fun F(this, stage, x, y, z)
    if stage == 0
      return bxor(bxor(x, y), z)
    else if stage == 1
      return bor(band(x, y), band(bnot(x), z))
    else if stage == 2
      return bxor(bor(x, bnot(y)), z)
    else if stage == 3
      return bor(band(x, z), band(y, bnot(z)))
    else
      return bxor(x, bor(y, bnot(z)))

  fun hex_val(this, ch)
    if ch == "0"
      return 0
    else if ch == "1"
      return 1
    else if ch == "2"
      return 2
    else if ch == "3"
      return 3
    else if ch == "4"
      return 4
    else if ch == "5"
      return 5
    else if ch == "6"
      return 6
    else if ch == "7"
      return 7
    else if ch == "8"
      return 8
    else if ch == "9"
      return 9
    else if ch == "a" || ch == "A"
      return 10
    else if ch == "b" || ch == "B"
      return 11
    else if ch == "c" || ch == "C"
      return 12
    else if ch == "d" || ch == "D"
      return 13
    else if ch == "e" || ch == "E"
      return 14
    else if ch == "f" || ch == "F"
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
      push(arr, this.byte_from_hex_pair(substr(hex, i, 2)))
      i = i + 2
    return arr

  fun two_hex(this, n)
    n = band(n, 0xFF)
    d = ["0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"]
    return join([d[shr(n, 4)], d[band(n, 0xF)]], "")

  fun bytes_to_hex(this, arr)
    i = 0
    out = []
    while i < len(arr)
      push(out, this.two_hex(arr[i]))
      i = i + 1
    return join(out, "")

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
      push(out, band(shr(len_bits, 8 * j), 0xFF))
      j = j + 1
    return out

  fun word32_le(this, b0, b1, b2, b3)
    return bor(bor(bor(b0, shl(b1, 8)), shl(b2, 16)), shl(b3, 24))

  fun process_block(this, state, block)
    M = []
    i = 0
    while i < 16
      j = i * 4
      push(M, this.word32_le(block[j], block[j+1], block[j+2], block[j+3]))
      i = i + 1
    
    A = state[0]
    B = state[1]
    C = state[2]
    D = state[3]
    E = state[4]
    Ar = state[0]
    Br = state[1]
    Cr = state[2]
    Dr = state[3]
    Er = state[4]
    
    j = 0
    while j < 80
      if j < 16
        stage = 0
        stage_r = 4
      else if j < 32
        stage = 1
        stage_r = 3
      else if j < 48
        stage = 2
        stage_r = 2
      else if j < 64
        stage = 3
        stage_r = 1
      else
        stage = 4
        stage_r = 0

      // Left side step
      T = this.add32(this.rol32(this.add32(this.add32(A, this.F(stage, B, C, D)), this.add32(M[this.RL[j]], this.KL[stage])), this.SL[j]), E)
      A = E
      E = D
      D = this.rol32(C, 10)
      C = B
      B = T

      // Right side step
      Tr = this.add32(this.rol32(this.add32(this.add32(Ar, this.F(stage_r, Br, Cr, Dr)), this.add32(M[this.RR[j]], this.KR[stage_r])), this.SR[j]), Er)
      Ar = Er
      Er = Dr
      Dr = this.rol32(Cr, 10)
      Cr = Br
      Br = Tr
      
      j = j + 1
    
    al = A
    bl = B
    cl = C
    dl = D
    el = E
    ar = Ar
    br = Br
    cr = Cr
    dr = Dr
    er = Er
    
    h0 = state[0]
    h1 = state[1]
    h2 = state[2]
    h3 = state[3]
    h4 = state[4]
    
    state[0] = this.add32(h1, this.add32(B, Cr))
    state[1] = this.add32(h2, this.add32(C, Dr))
    state[2] = this.add32(h3, this.add32(D, Er))
    state[3] = this.add32(h4, this.add32(E, Ar))
    state[4] = this.add32(h0, this.add32(A, Br))
    return state

  fun ripemd160_bytes(this, bytes)
    state = [1732584193, 4023233417, 2562383102, 271733878, 3285377520]
    data = this.pad_bytes(bytes)
    N = len(data)
    off = 0
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
    while j < 5
      v = state[j]
      push(out, band(v, 0xFF))
      push(out, band(shr(v, 8), 0xFF))
      push(out, band(shr(v, 16), 0xFF))
      push(out, band(shr(v, 24), 0xFF))
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
