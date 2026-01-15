#!/usr/bin/env fun

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

#include <crypt/ripemd160.fun>

fun to_hex_32(v)
  d = ["0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"]
  out = []
  i = 0
  while i < 4
    b = band(shr(v, (3 - i) * 8), 0xFF)
    push(out, d[shr(b, 4)])
    push(out, d[band(b, 0xF)])
    i = i + 1
  return join(out, "")

rmd = RIPEMD160()
state = [1732584193, 4023233417, 2562383102, 271733878, 3285377520]
block = []
push(block, 128) // 0x80
i = 1
while i < 64
  push(block, 0)
  i = i + 1

print("Initial state:")
for v in state
  print("  " + to_hex_32(v))

// Manual process_block logic with prints
M = []
i = 0
while i < 16
  j = i * 4
  push(M, rmd.word32_le(block[j], block[j+1], block[j+2], block[j+3]))
  i = i + 1

al = state[0]
bl = state[1]
cl = state[2]
dl = state[3]
el = state[4]
ar = state[0]
br = state[1]
cr = state[2]
dr = state[3]
er = state[4]

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

  f = rmd.F(stage, bl, cl, dl)
  TL1 = rmd.add32(al, f)
  TL2 = rmd.add32(M[rmd.RL[j]], rmd.KL[stage])
  TL3 = rmd.add32(TL1, TL2)
  TL4 = rmd.rol32(TL3, rmd.SL[j])
  TL = rmd.add32(TL4, el)
  if j < 2
    print("j=" + to_string(j) + " L: al=" + to_hex_32(al) + " f=" + to_hex_32(f) + " TL3=" + to_hex_32(TL3) + " TL4=" + to_hex_32(TL4) + " TL=" + to_hex_32(TL))
  al = el
  el = dl
  dl = rmd.rol32(cl, 10)
  cl = bl
  bl = TL

  fr = rmd.F(stage_r, br, cr, dr)
  TR1 = rmd.add32(ar, fr)
  TR2 = rmd.add32(M[rmd.RR[j]], rmd.KR[stage])
  TR3 = rmd.add32(TR1, TR2)
  TR4 = rmd.rol32(TR3, rmd.SR[j])
  TR = rmd.add32(TR4, er)
  if j < 2
    print("j=" + to_string(j) + " R: ar=" + to_hex_32(ar) + " fr=" + to_hex_32(fr) + " TR3=" + to_hex_32(TR3) + " TR4=" + to_hex_32(TR4) + " TR=" + to_hex_32(TR))
  ar = er
  er = dr
  dr = rmd.rol32(cr, 10)
  cr = br
  br = TR
  
  if j == 0
    print("After j=0:")
    print("  L: al=" + to_hex_32(al) + " bl=" + to_hex_32(bl) + " cl=" + to_hex_32(cl) + " dl=" + to_hex_32(dl) + " el=" + to_hex_32(el))
    print("  R: ar=" + to_hex_32(ar) + " br=" + to_hex_32(br) + " cr=" + to_hex_32(cr) + " dr=" + to_hex_32(dr) + " er=" + to_hex_32(er))
  
  j = j + 1

print("Registers after 80 steps:")
print("  L: A=" + to_hex_32(al) + " B=" + to_hex_32(bl) + " C=" + to_hex_32(cl) + " D=" + to_hex_32(dl) + " E=" + to_hex_32(el))
print("  R: Ar=" + to_hex_32(ar) + " Br=" + to_hex_32(br) + " Cr=" + to_hex_32(cr) + " Dr=" + to_hex_32(dr) + " Er=" + to_hex_32(er))

h0 = rmd.add32(state[1], rmd.add32(cl, dr))
h1 = rmd.add32(state[2], rmd.add32(dl, er))
h2 = rmd.add32(state[3], rmd.add32(el, ar))
h3 = rmd.add32(state[4], rmd.add32(al, br))
h4 = rmd.add32(state[0], rmd.add32(bl, cr))
state[0] = h0
state[1] = h1
state[2] = h2
state[3] = h3
state[4] = h4

print("Final state:")
for v in state
  print("  " + to_hex_32(v))

/* Possible output:
Initial state:
  67452301
  efcdab89
  98badcfe
  10325476
  c3d2e1f0
j=0 L: al=67452301 f=67452301 TL3=ce8a4682 TL4=52341674 TL=1606f864
j=0 R: ar=67452301 fr=10325476 TR3=c81a035d TR4=1a035dc8 TR=ddd63fb8
After j=0:
  L: al=c3d2e1f0 bl=1606f864 cl=efcdab89 dl=eb73fa62 el=10325476
  R: ar=c3d2e1f0 br=ddd63fb8 cr=efcdab89 dr=eb73fa62 er=10325476
j=1 L: al=c3d2e1f0 f=12b8a98f TL3=d68b8b7f TL4=e2dff5a2 TL=f3124a18
j=1 R: ar=c3d2e1f0 fr=221b9025 TR3=3690fdfb TR4=21fbf66d TR=322e4ae3
Registers after 80 steps:
  L: A=594637a7 B=215d97b3 C=eba027eb D=b99cdc7a E=ec4dcdff
  R: Ar=e7eb2e49 Br=85bff1f4 Cr=1d9bc99d Dr=a7ffa90b Er=ffcec761
Final state:
  836d7c7f
  522680d9
  e46b50be
  a2d90b8b
  a63e8451
*/
