#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

// PCSC stdlib abstraction class wrapping VM opcodes.
// Provides snake_case methods and hex helpers.
// All methods are defensive and work even if PCSC is unsupported (returning []/0 or a default map).

include <hex.fun>

/*
class PCSC()
  // ---- hex helpers moved to lib/hex.fun ----

  // ---- PCSC wrappers ----
  fun establish(this)
    // returns ctx id (>0) or 0
    return pcsc_establish()

  fun release(this, ctx)
    // returns 1 on success or 0
    return pcsc_release(ctx)

  fun list_readers(this, ctx)
    readers = pcsc_list_readers(ctx)
    if readers == nil
      return []
    return readers

  fun connect(this, ctx, reader_name)
    // returns handle id (>0) or 0
    return pcsc_connect(ctx, to_string(reader_name))

  fun disconnect(this, handle)
    // returns 1 on success or 0
    return pcsc_disconnect(handle)

  // Transmit raw APDU bytes (array of numbers 0..255). Returns map {"data":[], "sw1":n, "sw2":n, "code":n}
  fun transmit_bytes(this, handle, bytes)
    res = pcsc_transmit(handle, bytes)
    if res == nil
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    if res["data"] == nil
      res["data"] = []
    return res

  // Transmit hex APDU string, return map with hex "data_hex" and numeric sw1/sw2/code for convenience
  fun transmit_hex(this, handle, hex)
    arr = hex_to_bytes(hex)
    res = pcsc_transmit(handle, arr)
    if res == nil
      res = {}
      res["data"] = []
      res["sw1"] = -1
      res["sw2"] = -1
      res["code"] = -2
    if res["data"] == nil
      res["data"] = []
    dh = bytes_to_hex(res["data"])
    m = {}
    m["data_hex"] = dh
    m["sw1"] = res["sw1"]
    m["sw2"] = res["sw2"]
    m["code"] = res["code"]
    return m
*/

// This class is in a very early stage of development. 
class PCSC()
  fun get_readers(this)
    ctx = pcsc_establish()
    readers = pcsc_list_readers(ctx)
    _ = pcsc_release(ctx)
    return readers

  // Transmit a raw hex APDU to the first available reader and print the result map
  fun transmit(this, hex_apdu)
    ctx = pcsc_establish()
    readers = pcsc_list_readers(ctx)
    if (readers == nil)
      print([])
      _ = pcsc_release(ctx)
      // return a default result map instead of 0, so callers can index fields safely
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    if (len(readers) == 0)
      print([])
      _ = pcsc_release(ctx)
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    // Actually selecting the second reader hardcoded.
    handle = pcsc_connect(ctx, readers[1])
    apdu = this.hex_to_bytes(hex_apdu)
    res = pcsc_transmit(handle, apdu)
    _ = pcsc_disconnect(handle)
    _ = pcsc_release(ctx)
    return res

  fun hex_digit(this, ch)
    m = {}
    m["0"] = 0
    m["1"] = 1
    m["2"] = 2
    m["3"] = 3
    m["4"] = 4
    m["5"] = 5
    m["6"] = 6
    m["7"] = 7
    m["8"] = 8
    m["9"] = 9
    m["a"] = 10
    m["A"] = 10
    m["b"] = 11
    m["B"] = 11
    m["c"] = 12
    m["C"] = 12
    m["d"] = 13
    m["D"] = 13
    m["e"] = 14
    m["E"] = 14
    m["f"] = 15
    m["F"] = 15
    v = m[ch]
    if v == nil
      return 0
    return v

  fun hex_to_bytes(this, hex)
    s = to_string(hex)
    out = []
    number i = 0
    number n = len(s)
    while i + 1 < n
      number b = this.hex_digit(substr(s, i, 1)) * 16 + this.hex_digit(substr(s, i + 1, 1))
      push(out, b)
      i = i + 2
    return out
