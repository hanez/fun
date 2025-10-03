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

// Minimal PCSC example for Fun language
// Establish context and list readers (prints [] if none or unsupported)

// This example is very smartcard specific, but it works in my test environment.
// The card I am using is a german "eTicket" (local transport ticket). 

//include <hex.fun>
include <io/pcsc.fun>

p = PCSC()

readers = p.get_readers()
number rc = len(readers)

print("Found " + to_string(rc) + " readers:")

for i in range(0, rc)
  print("  " + to_string(i+1) + ". " + readers[i])
print("Dump:")
print(readers)

// Select Applet
string apdu = "00a4040c0cD2760001354B414E4D30310000"
print("Transmit APDU (Select Applet): " + apdu)
result = p.transmit(apdu)

print("Response:")
print("  Data: " + to_string(len(result["data"])) + " bytes")
print("  SW1:  " + to_string(result["sw1"]))
print("  SW2:  " + to_string(result["sw2"]))
print("  Code: " + to_string(result["code"]))
print("Dump:")
print(result)

// Call the random number generator (RNG)
apdu = "0084000008"
print("Transmit APDU (Call the random number generator (RNG): " + apdu)
result = p.transmit(apdu)

print("Response:")
print("  Data: " + to_string(len(result["data"])) + " bytes")
print("  SW1:  " + to_string(result["sw1"]))
print("  SW2:  " + to_string(result["sw2"]))
print("  Code: " + to_string(result["code"]))
print("Dump:")
print(result)

/* Possible output:
Found 2 readers:
  1. OMNIKEY CardMan (076B:5321) 5321 00 00
  2. OMNIKEY CardMan (076B:5321) 5321 00 01
Dump:
[OMNIKEY CardMan (076B:5321) 5321 00 00, OMNIKEY CardMan (076B:5321) 5321 00 01]
Transmit APDU (Select Applet): 00a4040c0cD2760001354B414E4D30310000
Response:
  Data: 251 bytes
  SW1:  144
  SW2:  0
  Code: 0
Dump:
{"data": [224, 129, 248, 226, 24, 192, 1, 1, 129, 15, 0, 0, 5, 250, 136, 243, 17, 36, 236, 105, 135, 46, 236, 105, 135, 128, 2, 7, 131, 228, 7, 192, 1, 2, 130, 2, 0, 162, 231, 3, 192, 1, 3, 236, 29, 192, 1, 4, 134, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 2, 0, 0, 195, 2, 1, 9, 144, 16, 9, 8, 7, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 233, 29, 192, 1, 9, 131, 20, 0, 0, 0, 1, 136, 243, 41, 106, 136, 142, 136, 42, 38, 33, 139, 156, 45, 30, 191, 125, 132, 2, 7, 1, 233, 29, 192, 1, 8, 131, 20, 0, 0, 0, 1, 136, 243, 41, 106, 136, 142, 136, 42, 38, 33, 139, 156, 45, 30, 191, 125, 132, 2, 7, 1, 233, 29, 192, 1, 7, 131, 20, 0, 0, 0, 1, 136, 243, 41, 106, 136, 142, 136, 42, 38, 33, 139, 156, 45, 30, 191, 125, 132, 2, 7, 1, 233, 29, 192, 1, 6, 131, 20, 0, 0, 15, 22, 136, 243, 31, 143, 136, 142, 136, 42, 37, 55, 0, 1, 47, 55, 191, 125, 132, 2, 7, 1, 233, 29, 192, 1, 5, 131, 20, 0, 0, 15, 21, 136, 243, 7, 216, 136, 143, 136, 42, 37, 55, 0, 1, 37, 58, 191, 125, 132, 2, 7, 1], "sw1": 144, "sw2": 0, "code": 0}
Transmit APDU (Call the random number generator (RNG): 0084000008
Response:
  Data: 8 bytes
  SW1:  144
  SW2:  0
  Code: 0
Dump:
{"data": [25, 179, 169, 196, 57, 36, 89, 139], "sw1": 144, "sw2": 0, "code": 0}
*/
