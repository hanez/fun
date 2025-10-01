/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

// Base64 encode/decode for byte arrays (RFC 4648, standard alphabet)

fun b64_encode_bytes(bytes)
  table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
  out = []
  number i = 0
  number n = len(bytes)
  while i + 2 < n
    number b0 = bytes[i]
    number b1 = bytes[i + 1]
    number b2 = bytes[i + 2]
    number v = b0 * 65536 + b1 * 256 + b2
    number c0 = (v / 262144) % 64     // bits 18..23
    number c1 = (v / 4096) % 64       // bits 12..17
    number c2 = (v / 64) % 64         // bits 6..11
    number c3 = v % 64                // bits 0..5
    push(out, substr(table, c0, 1))
    push(out, substr(table, c1, 1))
    push(out, substr(table, c2, 1))
    push(out, substr(table, c3, 1))
    i = i + 3

  number rem = n - i
  if (rem == 1)
    number b0 = bytes[i]
    number v = b0 * 65536
    number c0 = (v / 262144) % 64
    number c1 = (v / 4096) % 64
    push(out, substr(table, c0, 1))
    push(out, substr(table, c1, 1))
    push(out, "=")
    push(out, "=")
  else if (rem == 2)
    number b0 = bytes[i]
    number b1 = bytes[i + 1]
    number v = b0 * 65536 + b1 * 256
    number c0 = (v / 262144) % 64
    number c1 = (v / 4096) % 64
    number c2 = (v / 64) % 64
    push(out, substr(table, c0, 1))
    push(out, substr(table, c1, 1))
    push(out, substr(table, c2, 1))
    push(out, "=")

  return join(out, "")

fun b64_decode_to_bytes(s)
  table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
  str = to_string(s)
  out = []
  // build reverse lookup: index in table is value
  // Since we don't have a map, use find on each char
  number i = 0
  number n = len(str)
  while i < n
    // read 4 chars
    c0 = substr(str, i, 1)
    c1 = substr(str, i + 1, 1)
    c2 = substr(str, i + 2, 1)
    c3 = substr(str, i + 3, 1)
    number v0 = find(table, c0)
    number v1 = find(table, c1)
    number pad2 = (c2 == "=")
    number pad3 = (c3 == "=")
    number v2 = pad2 ? 0 : find(table, c2)
    number v3 = pad3 ? 0 : find(table, c3)
    number twentyfour = v0 * 262144 + v1 * 4096 + v2 * 64 + v3
    // bytes
    number b0 = (twentyfour / 65536) % 256
    number b1 = (twentyfour / 256) % 256
    number b2 = twentyfour % 256
    push(out, b0)
    if (!pad2)
      push(out, b1)
    if (!pad3)
      push(out, b2)
    i = i + 4
  return out
