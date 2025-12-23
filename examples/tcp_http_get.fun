/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * Simple TCP client in Fun: fetches HTTP/1.0 from example.org
 * Requires network access. Demonstrates tcp_connect, sock_send, sock_recv, sock_close.
 */

host = "example.org"
port = 80

fd = tcp_connect(host, port)
if (fd <= 0)
  print("connect failed")
else
  req = join([
    "GET / HTTP/1.0\r\n",
    "Host: ", host, "\r\n",
    "User-Agent: fun/0.20\r\n",
    "Connection: close\r\n\r\n"
  ], "")
  sent = sock_send(fd, req)
  // Read up to 8192 bytes (first chunk)
  data = sock_recv(fd, 8192)
  print(data)
  sock_close(fd)

/* Possible output:
HTTP/1.1 200 OK
Date: Thu, 18 Dec 2025 23:05:52 GMT
Content-Type: text/html
Connection: close
Vary: accept-encoding
Server: cloudflare
Last-Modified: Wed, 17 Dec 2025 03:51:06 GMT
Accept-Ranges: bytes
Cache-Control: max-age=14400
cf-cache-status: REVALIDATED
CF-RAY: 9b024eb71d91e51d-TXL

<!doctype html><html lang="en"><head><title>Example Domain</title><meta name="viewport" content="width=device-width, initial-scale=1"><style>body{background:#eee;width:60vw;margin:15vh auto;font-family:system-ui,sans-serif}h1{font-size:1.5em}div{opacity:0.8}a:link,a:visited{color:#348}</style><body><div><h1>Example Domain</h1><p>This domain is for use in documentation examples without needing permission. Avoid use in operations.<p><a href="https://iana.org/domains/example">Learn more</a></div></body></html>

*/

