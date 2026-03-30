#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-26
 */

//#include order kept minimal to avoid global bloat
#include <io/socket.fun>
#include <io/thread.fun>
// Note: Avoid relying on strings.fun helpers inside threads
#include <net/cgi.fun>

// Multi-threaded HTTP server with CGI support (thread-per-connection)
//
// Features:
// - Blocking accept() in the main thread
// - Each client is handled by a separate Fun thread
// - Serves static files from htdocs and executes .fun scripts via CGI helper
//
// Try:
//   http://127.0.0.1:8080/
//   http://127.0.0.1:8080/hello.fun?name=Fun
//   http://127.0.0.1:8080/info.fun

// Keep top-level globals minimal to avoid hitting the VM's global limit.

PORT = 8080
BACKLOG = 128
HTDOCS = "./examples/data/htdocs"

// Minimal local string helpers that avoid 'join' internally
fun _trim(s)
  src = to_string(s)
  // ltrim
  i = 0
  ws = " \t\r\n"
  while (i < len(src))
    ch = substr(src, i, 1)
    if (find(ws, ch) < 0)
      break
    i = i + 1
  left = substr(src, i, len(src) - i)
  // rtrim
  j = len(left) - 1
  while (j >= 0)
    ch2 = substr(left, j, 1)
    if (find(ws, ch2) < 0)
      break
    j = j - 1
  return substr(left, 0, j + 1)

fun _ends_with(s, suf)
  a = to_string(s)
  p = to_string(suf)
  la = len(a)
  lp = len(p)
  if (lp > la)
    return 0
  return substr(a, la - lp, lp) == p

// Split by single character delimiter (first char of delim string)
fun _split_char(s, delim)
  src = to_string(s)
  d = to_string(delim)
  if (len(d) == 0)
    return [src]
  dd = substr(d, 0, 1)
  parts = []
  buf = ""
  i = 0
  n = len(src)
  while (i < n)
    ch = substr(src, i, 1)
    if (ch == dd)
      push(parts, buf)
      buf = ""
    else
      buf = buf + ch
    i = i + 1
  push(parts, buf)
  return parts

fun _split_space(s)
  return _split_char(s, " ")

fun _split_lines(s)
  return _split_char(s, "\n")

fun _to_upper_ascii(s)
  src = to_string(s)
  U = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  L = "abcdefghijklmnopqrstuvwxyz"
  out = ""
  i = 0
  n = len(src)
  while (i < n)
    ch = substr(src, i, 1)
    idx = find(L, ch)
    if (idx >= 0)
      out = out + substr(U, idx, 1)
    else
      out = out + ch
    i = i + 1
  return out

// Minimal send helpers (few identifiers)
fun _send(fd, code, text, body)
  b = to_string(body)
  resp = "HTTP/1.1 " + to_string(code) + " " + text + "\r\n"
  resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
  resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
  resp = resp + "Connection: close\r\n\r\n" + b
  sock_send(fd, resp)

// Convert CGI raw output (headers + body) to full HTTP/1.1 response without using strings.fun helpers
fun _cgi_to_http_response(raw)
  out = to_string(raw)
  // Find header/body separator
  sep = find(out, "\r\n\r\n")
  seplen = 4
  if (sep < 0)
    sep = find(out, "\n\n")
    seplen = 2
  if (sep < 0)
    // No CGI headers, treat whole as body
    b = out
    resp = "HTTP/1.1 200 OK\r\n"
    resp = resp + "Content-Type: text/html; charset=utf-8\r\n"
    resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
    resp = resp + "Connection: close\r\n\r\n" + b
    return resp

  header_str = substr(out, 0, sep)
  body = substr(out, sep + seplen, len(out) - sep - seplen)

  // Parse headers line-by-line
  lines = _split_lines(header_str)
  code = 200
  text = "OK"
  hh = []  // [key, value]
  i = 0
  if (typeof(lines) == "Array")
    while (i < len(lines))
      ln = _trim(lines[i])
      if (len(ln) > 0)
        colon = find(ln, ":")
        if (colon > 0)
          k = _trim(substr(ln, 0, colon))
          v = _trim(substr(ln, colon + 1, len(ln) - colon - 1))
          if (_to_upper_ascii(k) == "STATUS")
            sp = find(v, " ")
            if (sp > 0)
              code = to_number(substr(v, 0, sp))
              text = _trim(substr(v, sp + 1, len(v) - sp - 1))
            else
              code = to_number(v)
              if (code == 0) code = 200
              text = "OK"
          else
            push(hh, [k, v])
      i = i + 1

  // Build HTTP response
  b = to_string(body)
  resp = "HTTP/1.1 " + to_string(code) + " " + text + "\r\n"
  j = 0
  m = len(hh)
  has_len = 0
  while (j < m)
    p = hh[j]
    if (typeof(p) == "Array" && len(p) >= 2)
      hk = to_string(p[0])
      hv = to_string(p[1])
      if (_to_upper_ascii(hk) == "CONTENT-LENGTH")
        has_len = 1
      resp = resp + hk + ": " + hv + "\r\n"
    j = j + 1
  if (!has_len)
    resp = resp + "Content-Length: " + to_string(len(b)) + "\r\n"
  resp = resp + "Connection: close\r\n\r\n" + b
  return resp

fun _send_cgi(fd, raw)
  resp = _cgi_to_http_response(raw)
  sock_send(fd, resp)

// Worker: parse request and serve static or .fun via CGI
fun handle_client(fd)
  req = sock_recv(fd, 65536)
  if (len(req) == 0)
    sock_close(fd)
    return 0

  s = to_string(req)
  nl = find(s, "\n")
  if (nl < 0)
    nl = find(s, "\r")
  if (nl < 0)
    sock_close(fd)
    return 0
  line = _trim(substr(s, 0, nl))
  ps = _split_space(line)
  if (!(typeof(ps) == "Array") || len(ps) < 2)
    sock_close(fd)
    return 0
  method = _trim(ps[0])
  target = _trim(ps[1])

  // path + query
  path = target
  query = ""
  q = find(target, "?")
  if (q >= 0)
    path = substr(target, 0, q)
    query = substr(target, q + 1, len(target) - q - 1)
  if (path == "/")
    path = "/index.html"

  htdocs = env("FUN_HTDOCS")
  if (htdocs == nil || len(htdocs) == 0)
    htdocs = HTDOCS
  file = htdocs + path

  // parse headers small loop
  headers = {}
  lines = _split_lines(s)
  if (typeof(lines) == "Array")
    i = 1
    while (i < len(lines))
      ln = _trim(lines[i])
      if (len(ln) == 0)
        break
      cpos = find(ln, ":")
      if (cpos > 0)
        k = _to_upper_ascii(_trim(substr(ln, 0, cpos)))
        v = _trim(substr(ln, cpos + 1, len(ln) - cpos - 1))
        headers[k] = v
      i = i + 1

  // body (optional)
  body = ""
  hend = find(s, "\r\n\r\n")
  if (hend >= 0)
    body = substr(s, hend + 4, len(s) - hend - 4)
  else
    hend = find(s, "\n\n")
    if (hend >= 0)
      body = substr(s, hend + 2, len(s) - hend - 2)

  if (_ends_with(path, ".fun"))
    host = headers["HOST"]
    if (host == nil || len(host) == 0)
      host = "localhost"
    ua = headers["USER-AGENT"]
    cookie = headers["COOKIE"]
    ctype = headers["CONTENT-TYPE"]
    clen = headers["CONTENT-LENGTH"]

    envs = []
    funlib = env("FUN_LIB_DIR")
    if (len(funlib) == 0)
      funlib = "./lib"
    push(envs, "FUN_LIB_DIR='" + funlib + "'")
    push(envs, "REQUEST_METHOD='" + method + "'")
    push(envs, "QUERY_STRING='" + query + "'")
    push(envs, "SCRIPT_NAME='" + path + "'")
    push(envs, "PATH_INFO='" + path + "'")
    push(envs, "SERVER_NAME='" + host + "'")
    pstr = env("FUN_PORT")
    if (pstr == nil || len(pstr) == 0)
      pstr = to_string(PORT)
    push(envs, "SERVER_PORT='" + pstr + "'")
    push(envs, "SERVER_PROTOCOL='HTTP/1.1'")
    push(envs, "HTTP_HOST='" + host + "'")
    if (!(ua == nil) && len(ua) > 0)
      push(envs, "HTTP_USER_AGENT='" + ua + "'")
    if (!(cookie == nil) && len(cookie) > 0)
      push(envs, "HTTP_COOKIE='" + cookie + "'")
    if (!(ctype == nil) && len(ctype) > 0)
      push(envs, "CONTENT_TYPE='" + ctype + "'")
    if (!(clen == nil) && len(clen) > 0)
      push(envs, "CONTENT_LENGTH='" + clen + "'")
    if (len(body) > 0)
      push(envs, "POST_DATA='" + body + "'")

    exec = env("FUN_EXEC")
    if (len(exec) == 0)
      if (len(read_file("./build_debug/fun")) > 0)
        exec = "./build_debug/fun"
      else
        if (len(read_file("./build_release/fun")) > 0)
          exec = "./build_release/fun"
        else
          exec = "fun"

    // Manually concatenate env exports to avoid join()
    envs_str = ""
    ei = 0
    en = len(envs)
    while (ei < en)
      if (ei > 0)
        envs_str = envs_str + " "
      envs_str = envs_str + envs[ei]
      ei = ei + 1
    cmd = envs_str + " " + exec + " " + file
    res = proc_run(cmd)
    if (res == nil || typeof(res) != "Map")
      _send(fd, 500, "Internal Server Error", "<h1>Failed to execute CGI</h1>")
      sock_close(fd)
      return 0
    out = res["out"]
    if (out == nil)
      out = ""
    code = res["code"]
    if (code == nil)
      code = 0
    if (len(out) == 0)
      if (to_string(code) != "0")
        _send(fd, 500, "Internal Server Error", "<h1>CGI failed (exit " + to_string(code) + ")</h1>")
      else
        _send(fd, 500, "Internal Server Error", "<h1>CGI produced no output</h1>")
    else
      _send_cgi(fd, out)
  else
    content = read_file(file)
    if (len(content) > 0)
      _send(fd, 200, "OK", content)
    else
      _send(fd, 404, "Not Found", "<h1>404 Not Found</h1>")

  sock_close(fd)
  return 1

// Setup server and accept loop
srv = TcpServer(PORT, BACKLOG)
if (srv.listen() <= 0)
  print("HTTP MT CGI server: listen failed on :" + to_string(PORT))
  return 0
print("HTTP MT CGI server: serving " + HTDOCS + " on :" + to_string(PORT))

th = Thread()
while true
  fd = srv.accept()
  if (fd > 0)
    _ = th.spawn(handle_client, fd)

/*
Expected output (on start):
HTTP MT CGI server: serving ./examples/data/htdocs on :8080

Then open a browser:
  - http://127.0.0.1:8080/
  - http://127.0.0.1:8080/hello.fun?name=Fun
*/
