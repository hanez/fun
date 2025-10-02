#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

// __ns_alias__: PCSC

// PCSC stdlib abstraction class wrapping VM opcodes.
// Provides snake_case methods and hex helpers.
// All methods are defensive and work even if PCSC is unsupported (returning []/0 or a default map).

#include <hex.fun>

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
