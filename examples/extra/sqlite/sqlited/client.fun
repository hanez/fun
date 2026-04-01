#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-19
 */

// Simple TCP SQL client for Fun
// Connects to host:port, sends a single-line SQL (from CLI args or default),
// prints the server response, and exits.

// Run the server:
// FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlited/server.fun 127.0.0.1 5555

// Run the client:
// FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error ./examples/sqlited/client.fun 127.0.0.1 5555 "SELECT * FROM tasks"

#include <cli.fun>

fun arg_or_default(args, i, d)
  if (len(args) > i)
    return args[i]
  else
    return d

fun read_all(fd)
  buf = ""
  while (true)
    chunk = sock_recv(fd, 1024)
    if (chunk == nil || len(chunk) == 0)
      break
    buf = buf + chunk
  return buf

fun main()
  args = argv()
  host = arg_or_default(args, 0, "127.0.0.1")
  port = to_number(arg_or_default(args, 1, 5555))
  sql  = arg_or_default(args, 2, "SELECT 1 AS one;")

  fd = tcp_connect(host, port)
  if (fd == 0)
    print("Connect failed to " + host + " " + to_string(port))
    return 1

  // Ensure a single line terminated by \n
  if (len(sql) == 0 || substr(sql, len(sql)-1, 1) != "\n")
    sql = sql + "\n"

  sent = sock_send(fd, sql)
  if (sent < 0)
    print("Send failed")
    sock_close(fd)
    return 1

  resp = read_all(fd)
  sock_close(fd)
  if (resp == nil)
    resp = ""
  print(resp)

// Explicitly invoke main when the script is run
main()

/* Possible result with 67 entries in the tasks table:
RESULT
value
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
{map n=4}
END
*/
