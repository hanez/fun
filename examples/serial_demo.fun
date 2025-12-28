#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

#include <io/serial.fun>

// This is an example of how to use the Serial class from the stdlib.
// Note: This requires a serial device to be present at the specified path.

path = "/dev/ttyUSB0"
baud = 115200

print("Opening serial port " + path + " at " + to_string(baud) + " baud...")
s = Serial(path, baud)

if (s.open())
  print("Serial port opened successfully.")

  // Configure: 8 data bits, no parity (0), 1 stop bit, no flow control (0)
  if (s.config(8, 0, 1, 0))
    print("Configured to 8N1.")

    sent = s.send("AT\r\n")
    print("Sent " + to_string(sent) + " bytes.")

    // Wait a bit for response if needed (mocked by sleep if available)
    // sleep_ms(100)

    resp = s.recv(64)
    if (len(resp) > 0)
      print("Received: " + resp)
    else
      print("No response received.")
  else
    print("Failed to configure serial port.")

  s.close()
  print("Serial port closed.")
else
  print("Failed to open serial port. (Do you have permissions?)")

/* Possible output:
Opening serial port /dev/ttyUSB0 at 115200 baud...
Failed to open serial port. (Do you have permissions?)
*/
