/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

/*
 * Serial communication class for Fun stdlib.
 *
 * Provides:
 * - Serial: wrapper over built-ins: serial_open, serial_config, serial_send, serial_recv, serial_close
 *
 * Usage:
 *   #include <io/serial.fun>
 *   s = Serial("/dev/ttyUSB0", 115200)
 *   if (s.open())
 *     s.config(8, 0, 1, 0) // 8N1, no flow control
 *     s.send("Hello Serial!")
 *     resp = s.recv(64)
 *     print("Received: " + resp)
 *     s.close()
 */

class Serial(string path, number baud_rate)

  fun _construct(this, path, baud_rate)
    this.path = path
    this.baud_rate = baud_rate
    this.fd = 0

  fun open(this)
    this.fd = serial_open(this.path, this.baud_rate)
    return this.fd > 0

  fun is_open(this)
    return this.fd > 0

  fun config(this, data_bits, parity, stop_bits, flow_control)
    if (!this.is_open())
      return 0
    // parity: 0=None, 1=Odd, 2=Even
    // flow_control: 0=None, 1=Hardware (RTS/CTS)
    return serial_config(this.fd, data_bits, parity, stop_bits, flow_control)

  fun send(this, data)
    if (!this.is_open())
      return -1
    return serial_send(this.fd, to_string(data))

  fun recv(this, maxlen)
    if (!this.is_open())
      return ""
    return serial_recv(this.fd, to_number(maxlen))

  fun close(this)
    if (this.is_open())
      res = serial_close(this.fd)
      this.fd = 0
      return res
    return 0
