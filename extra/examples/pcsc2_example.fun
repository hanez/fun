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

// Example using stdlib PCSC2 class.
// Establish context, list readers, connect to first (if any),
// send a sample SELECT MF APDU, print results, and clean up.

#include <io/pcsc2.fun>

pc = PCSC2()

ctx = pc.establish()
print("ctx=" + to_string(ctx))

readers = pc.list_readers(ctx)
print("readers=" + to_string(readers))

number h = 0
if len(readers) > 0
  h = pc.connect(ctx, readers[1])
  print("handle=" + to_string(h))

  if h != 0
    // APDU 1: SELECT applet by AID (should return 251 data bytes, constant)
    resp = pc.transmit_hex(h, "00a4040c0cD2760001354B414E4D30310000")
    print(to_string(resp))
    print("resp.data_hex=" + resp["data_hex"]) 
    print("resp.sw1=" + to_string(resp["sw1"]) + " sw2=" + to_string(resp["sw2"]) + " code=" + to_string(resp["code"]))
    number dlen1 = len(resp["data_hex"]) / 2
    print("SELECT AID data length=" + to_string(dlen1) + " (expected 251)")

    // APDU 2: GET CHALLENGE 8 (should return 8 random bytes)
    resp = pc.transmit_hex(h, "0084000008")
    print(to_string(resp))
    print("resp.data_hex=" + resp["data_hex"]) 
    print("resp.sw1=" + to_string(resp["sw1"]) + " sw2=" + to_string(resp["sw2"]) + " code=" + to_string(resp["code"]))
    number dlen2 = len(resp["data_hex"]) / 2
    print("GET CHALLENGE data length=" + to_string(dlen2) + " (expected 8)")

    _ = pc.disconnect(h)

_ = pc.release(ctx)
print("done")
