#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

// PCSC2 stdlib abstraction class wrapping VM PC/SC opcodes.
// Goal: Provide clean, object-oriented access to PC/SC with safe fallbacks
// when the VM is built without PCSC support. All methods avoid throwing and
// return neutral values (0, [], or a default map).

include <hex.fun>

class PCSC2()
  // Establish a new PC/SC context. Returns ctx id (>0) or 0 if unavailable.
  fun establish(this)
    res = pcsc_establish()
    if res == nil
      return 0
    return res

  // Release a PC/SC context. Returns 1 on success, 0 otherwise.
  fun release(this, ctx)
    res = pcsc_release(ctx)
    if res == nil
      return 0
    return res

  // List available reader names for a context. Returns [] on failure.
  fun list_readers(this, ctx)
    readers = pcsc_list_readers(ctx)
    if readers == nil
      return []
    return readers

  // Connect to a given reader name for a context. Returns handle id (>0) or 0.
  fun connect(this, ctx, reader_name)
    res = pcsc_connect(ctx, to_string(reader_name))
    if res == nil
      return 0
    return res

  // Disconnect a card handle. Returns 1 on success or 0.
  fun disconnect(this, handle)
    res = pcsc_disconnect(handle)
    if res == nil
      return 0
    return res

  // Transmit raw APDU bytes (array of numbers 0..255).
  // Returns map {"data":[], "sw1":n, "sw2":n, "code":n}
  fun transmit_bytes(this, handle, bytes)
    res = pcsc_transmit(handle, bytes)
    // Normalize to map regardless of VM build
    if res == nil
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    t = typeof(res)
    if t != "Map"
      m = {}
      m["data"] = []
      m["sw1"] = -1
      m["sw2"] = -1
      m["code"] = -2
      return m
    if res["data"] == nil
      res["data"] = []
    return res

  // Transmit hex APDU string. Returns a convenience map with hex payload too:
  // {"data_hex":string, "sw1":n, "sw2":n, "code":n}
  fun transmit_hex(this, handle, hex)
    arr = hex_to_bytes(hex)
    res = pcsc_transmit(handle, arr)
    // Normalize result to a map first
    number sw1 = -1
    number sw2 = -1
    number code = -2
    data_arr = []
    if res != nil && typeof(res) == "Map"
      if res["sw1"] != nil
        sw1 = res["sw1"]
      if res["sw2"] != nil
        sw2 = res["sw2"]
      if res["code"] != nil
        code = res["code"]
      if res["data"] != nil
        data_arr = res["data"]
    dh = bytes_to_hex(data_arr)
    m = {}
    m["data_hex"] = dh
    m["sw1"] = sw1
    m["sw2"] = sw2
    m["code"] = code
    return m
