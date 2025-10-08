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

// PCSC demo using stdlib PCSC class.
// Establishes context, lists readers, optionally connects to the first,
// transmits a sample APDU (SELECT MF), and cleans up.

#include <io/pcsc.fun>

pc = PCSC()

ctx = pc.establish()
print("ctx=" + to_string(ctx))

readers = pc.list_readers(ctx)
print("readers=" + to_string(readers))

number h = 0
if len(readers) > 0
  // Connect to first reader
  h = pc.connect(ctx, readers[1])
  print("handle=" + to_string(h))

  if h != 0
    // Sample APDU: SELECT MF (00 A4 00 00 02 3F 00)
    resp = pc.transmit_hex(h, "00A40000023F00")
    // Print hex data and SW
    print("resp.data_hex=" + resp["data_hex"])
    print("resp.sw1=" + to_string(resp["sw1"]) + " sw2=" + to_string(resp["sw2"]) + " code=" + to_string(resp["code"]))

    _ = pc.disconnect(h)

_ = pc.release(ctx)
print("done")
