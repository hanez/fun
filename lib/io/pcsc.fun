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

// Unified PCSC class API
class PCSC()
  // ---- Context management ----
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

  // Convenience: get readers without managing ctx externally
  fun get_readers(this)
    ctx = pcsc_establish()
    readers = pcsc_list_readers(ctx)
    _ = pcsc_release(ctx)
    if readers == nil
      return []
    return readers

  // ---- Connection management ----
  fun connect(this, ctx, reader_name)
    // returns handle id (>0) or 0
    return pcsc_connect(ctx, to_string(reader_name))

  fun disconnect(this, handle)
    // returns 1 on success or 0
    return pcsc_disconnect(handle)

  // ---- Transmit helpers ----
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

  // Convenience: Transmit a raw hex APDU to the first available reader and return the result map
  fun transmit(this, hex_apdu)
    ctx = pcsc_establish()
    readers = pcsc_list_readers(ctx)
    if (readers == nil)
      _ = pcsc_release(ctx)
      // return a default result map instead of 0, so callers can index fields safely
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    if (len(readers) == 0)
      _ = pcsc_release(ctx)
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    // Select the second reader (index 1) as previously used in examples
    handle = pcsc_connect(ctx, readers[1])
    apdu_bytes = hex_to_bytes(hex_apdu)
    res = pcsc_transmit(handle, apdu_bytes)
    _ = pcsc_disconnect(handle)
    _ = pcsc_release(ctx)
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
