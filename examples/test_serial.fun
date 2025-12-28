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

// Test script for serial communication
// This script attempts to open a virtual serial port if it exists,
// or just demonstrates the syntax if not.

fun test_serial()
  number fd = 0
  number baud = 9600
  string path = "/dev/ttyUSB0"

  print("Attempting to open serial port: " + path)
  fd = serial_open(path, baud)

  if fd > 0
    print("Successfully opened serial port. FD: " + to_string(fd))

    // Config: fd, data_bits, parity, stop_bits, flow_control
    // Parity: 0=None, 1=Odd, 2=Even
    // Flow: 0=None, 1=Hardware
    number ok = serial_config(fd, 8, 0, 1, 0)
    if ok
      print("Configured serial port: 8N1, no flow control")

      number sent = serial_send(fd, "HELLO SERIAL\n")
      print("Sent " + to_string(sent) + " bytes")

      // recv is blocking in this implementation
      // data = serial_recv(fd, 100)
      // print "Received: " + data
    else
      print("Failed to configure serial port")

    serial_close(fd)
    print("Closed serial port")
  else
    print("Could not open serial port (this is expected if /dev/ttyUSB0 doesn't exist or no permission)")

test_serial()

/* Possible output:
Attempting to open serial port: /dev/ttyUSB0
Could not open serial port (this is expected if /dev/ttyUSB0 doesn't exist or no permission)
*/
