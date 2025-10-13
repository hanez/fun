/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * Sockets convenience classes for Fun stdlib.
 *
 * Provides:
 * - TcpClient: simple TCP client wrapper over built-ins: tcp_connect, sock_send, sock_recv, sock_close
 * - UnixClient: simple UNIX-domain client wrapper over built-ins: unix_connect, sock_send, sock_recv, sock_close
 *
 * Usage:
 *   #include <io/socket.fun>
 *   c = TcpClient()
 *   if (c.connect("example.org", 80))
 *     c.send("GET / HTTP/1.0\r\nHost: example.org\r\n\r\n")
 *     resp = c.recv_all(8192)
 *     print(resp)
 *     c.close()
 */
class TcpClient()
  // file descriptor or 0 if not connected
  fd = 0

  fun connect(this, host, port)
    // host: string, port: int
    this.fd = tcp_connect(host, port)
    return this.fd > 0

  fun is_connected(this)
    return this.fd > 0

  fun send(this, data)
    if (!this.is_connected())
      return -1
    // returns bytes sent or -1
    return sock_send(this.fd, to_string(data))

  fun recv(this, maxlen)
    if (!this.is_connected())
      return ""
    // returns string (empty on EOF/error)
    return sock_recv(this.fd, to_number(maxlen))

  // Receive until EOF or up to max_total bytes (0 => unlimited, but capped inside recv)
  fun recv_all(this, chunk_size)
    cs = to_number(chunk_size)
    if (cs <= 0)
      cs = 4096
    buf = ""
    while this.is_connected()
      part = this.recv(cs)
      if (len(part) == 0)
        break
      buf = join([buf, part], "")
      // crude heuristic: stop if last chunk was smaller than requested
      if (len(part) < cs)
        break
    return buf

  fun close(this)
    if (this.is_connected())
      sock_close(this.fd)
      this.fd = 0
    return 1

// TcpServer: simple wrapper around TCP socket built-ins.
// Exposes listen(), accept(), echo_once(maxlen), serve_forever(maxlen), close().
class TcpServer(number port, number backlog)

  fun _construct(this, port, backlog)
    this.port = port
    this.backlog = backlog
    this.listen_fd = 0

  fun listen(this)
    this.listen_fd = tcp_listen(this.port, this.backlog)
    return this.listen_fd

  fun accept(this)
    return tcp_accept(this.listen_fd)

  fun close(this)
    if (this.listen_fd > 0)
      sock_close(this.listen_fd)
      this.listen_fd = 0

  fun echo_once(this, maxlen)
    c = this.accept()
    if (c <= 0)
      return 0

    data = sock_recv(c, maxlen)
    if (len(data) > 0)
      sock_send(c, data)

    sock_close(c)
    return 1

  fun serve_forever(this, maxlen)
    if (this.listen_fd <= 0)
      if (this.listen() <= 0)
        print("TcpServer: listen failed on port " + to_string(this.port))
        return 0
      print("TcpServer: listening on port " + to_string(this.port))
    while (true)
      this.echo_once(maxlen)

class UnixClient()
  fd = 0

  fun connect(this, path)
    this.fd = unix_connect(to_string(path))
    return this.fd > 0

  fun is_connected(this)
    return this.fd > 0

  fun send(this, data)
    if (!this.is_connected())
      return -1
    return sock_send(this.fd, to_string(data))

  fun recv(this, maxlen)
    if (!this.is_connected())
      return ""
    return sock_recv(this.fd, to_number(maxlen))

  fun close(this)
    if (this.is_connected())
      sock_close(this.fd)
      this.fd = 0
    return 1
