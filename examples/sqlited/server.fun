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

// Simple TCP SQL server for Fun
// Listens on a TCP port, opens ./database.sqlite, executes one-line SQL per connection,
// and returns results over the socket in a simple TSV protocol.

// Run the server:
// FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlited/server.fun 127.0.0.1 5555

// Run the client:
// FUN_LIB_DIR="$(pwd)/lib" ./build/fun --repl-on-error ./examples/sqlited/client.fun 127.0.0.1 5555 "SELECT * FROM tasks"

// Protocol (per protocol.md):
// - Client sends a single line of SQL ending with \n
// - If query returns rows: respond with
//     RESULT\n
//     <col1>\t<col2>\t...\n
//     <v11>\t<v12>\t...\n
//     ...
//     END\n
// - If exec/DDL: respond with
//     OK <rc>\n
// - On error: respond with
//     ERROR <message>\n

// Helper: CLI args via stdlib
#include <cli.fun>
#include <strings.fun>

fun arg_or_default(args, i, d)
  if (len(args) > i)
    return args[i]
  else
    return d

// Helper: send a string (no newline added)
fun send(fd, s)
  // sock_send returns bytes or -1
  return sock_send(fd, s)

// Helper: read a single line (up to max_len) ending with \n; returns string without trailing \r?\n or nil on EOF
fun read_line(fd)
  max_len = 65536
  buf = ""
  while (len(buf) < max_len)
    chunk = sock_recv(fd, 256)
    if (chunk == nil || len(chunk) == 0)
      break
    buf = buf + chunk
    pos = find(buf, "\n")
    if (pos >= 0)
      line = substr(buf, 0, pos)
      // trim trailing \r if present
      if (len(line) > 0 && substr(line, len(line)-1, 1) == "\r")
        line = substr(line, 0, len(line)-1)
      return line
  if (len(buf) == 0)
    return nil
  // no newline; return whole buffer (trim any trailing CR)
  if (len(buf) > 0 && substr(buf, len(buf)-1, 1) == "\r")
    buf = substr(buf, 0, len(buf)-1)
  return buf

// Replace tab/newline with spaces for TSV safety
fun sanitize_tsv(s)
  if (s == nil)
    return ""
  out = ""
  i = 0
  while (i < len(s))
    ch = substr(s, i, 1)
    if (ch == "\t" || ch == "\n" || ch == "\r")
      out = out + " "
    else
      out = out + ch
    i = i + 1
  return out

fun trim(s)
  // trim spaces and tabs
  i = 0
  j = len(s)
  while (i < j && (substr(s, i, 1) == " " || substr(s, i, 1) == "\t"))
    i = i + 1
  while (j > i && (substr(s, j-1, 1) == " " || substr(s, j-1, 1) == "\t" || substr(s, j-1, 1) == ";"))
    j = j - 1
  return substr(s, i, j - i)

fun split_on_comma(s)
  parts = []
  cur = ""
  i = 0
  while (i < len(s))
    ch = substr(s, i, 1)
    if (ch == ",")
      push(parts, trim(cur))
      cur = ""
    else
      cur = cur + ch
    i = i + 1
  push(parts, trim(cur))
  return parts

// Parse header from SQL SELECT list; for SELECT * tries PRAGMA table_info(table)
fun parse_header_from_sql(sql, dbh)
  // Use stdlib helper for lowercase
  lower_sql = str_to_lower(sql)
  psel = find(lower_sql, "select ")
  pfrom = find(lower_sql, " from ")
  if (psel < 0 || pfrom < 0 || pfrom <= psel)
    return nil
  cols_str = substr(sql, psel + 7, pfrom - (psel + 7))
  cols_str = trim(cols_str)
  if (find(cols_str, "*") >= 0)
    // Attempt to detect table name after FROM
    rest = substr(sql, pfrom + 6, len(sql) - (pfrom + 6))
    rest = trim(rest)
    // table name is up to next space or semicolon
    sp = find(rest, " ")
    tname = rest
    if (sp > 0)
      tname = substr(rest, 0, sp)
    // remove trailing semicolon if any
    tname = trim(tname)
    if (len(tname) > 0)
      pragma_sql = "PRAGMA table_info(" + tname + ");"
      ti = sqlite_query(dbh, pragma_sql)
      if (ti != nil && len(ti) > 0)
        cols = []
        i = 0
        while (i < len(ti))
          nm = ti[i]["name"]
          if (nm != nil)
            push(cols, to_string(nm))
          i = i + 1
        if (len(cols) > 0)
          return cols
  // Parse explicit column list
  parts = split_on_comma(cols_str)
  cols = []
  i = 0
  while (i < len(parts))
    p = parts[i]
    pl = lower(p)
    // handle AS alias
    aspos = find(pl, " as ")
    if (aspos >= 0)
      alias = trim(substr(p, aspos + 4, len(p) - (aspos + 4)))
      push(cols, alias)
    else
      // take last token after dot
      dot = find(p, ".")
      if (dot >= 0)
        push(cols, trim(substr(p, dot + 1, len(p) - (dot + 1))))
      else
        push(cols, trim(p))
    i = i + 1
  if (len(cols) > 0)
    return cols
  return nil

// Attempt to build a deterministic header and row order using enumerate(row).
// Falls back to attempting common column names if enumerate is unavailable.
fun extract_header(row)
  // Build a header by probing a set of common keys present in many queries.
  // If none are present, fall back to a single synthetic column "value" and
  // the caller will print the entire row using to_string(row).
  hdr_candidates = [
    "id", "name", "title", "value", "count", "cnt",
    "done", "created_at", "updated_at", "rowid"
  ]
  cols = []
  found = 0
  i = 0
  while (i < len(hdr_candidates))
    k = hdr_candidates[i]
    v = row[k]
    if (v != nil)
      push(cols, k)
      found = 1
    i = i + 1
  if (found == 1)
    return [cols, 0]  // is_synthetic = 0
  else
    return [["value"], 1]  // is_synthetic = 1

// Try to obtain map keys via enumerate(row). Returns [keys, is_synthetic]
fun header_from_enumerate(row)
  keys = []
  pairs = enumerate(row)
  if (pairs == nil)
    return [["value"], 1]
  i = 0
  while (i < len(pairs))
    p = pairs[i]
    // Expect pair to be [key, value]
    if (p != nil && len(p) >= 1)
      push(keys, p[0])
    i = i + 1
  if (len(keys) == 0)
    return [["value"], 1]
  return [keys, 0]

fun handle_client(fd, dbh)
  print("[sqlited] client connected: fd=" + to_string(fd))
  sql = read_line(fd)
  print("[sqlited] received SQL: '" + (sql == nil ? "" : sql) + "'")
  if (sql == nil || len(sql) == 0)
    send(fd, "ERROR empty\n")
    sock_close(fd)
    return 0

  // Try query first
  rows = sqlite_query(dbh, sql)
  if (rows != nil)
    print("[sqlited] query path; rows array obtained")
    // Build response in the stable synthetic format used in the 5558 build:
    // RESULT\n
    // value\n
    // {map n=...}\n (per row)
    resp = "RESULT\n"
    // Always emit single-column header 'value' for compatibility
    resp = resp + "value\n"
    // Emit rows
    r = 0
    while (r < len(rows))
      row = rows[r]
      print("[sqlited] sending row #" + to_string(r))
      resp = resp + sanitize_tsv(to_string(row)) + "\n"
      print("[sqlited] row #" + to_string(r) + " appended (synth)")
      r = r + 1
    // Terminate block
    print("[sqlited] finished building response; sending END and closing")
    resp = resp + "END\n"
    sb = send(fd, resp)
    print("[sqlited] total bytes sent=" + to_string(sb))
    sock_close(fd)
    return 1
  else
    // Exec path
    print("[sqlited] exec/DDL path")
    rc = sqlite_exec(dbh, sql)
    print("[sqlited] exec rc=" + to_string(rc))
    send(fd, "OK " + to_string(rc) + "\n")
    sock_close(fd)
    return 1

fun main()
  args = argv()
  host = arg_or_default(args, 0, "127.0.0.1")
  port = to_number(arg_or_default(args, 1, 5555))

  dbh = sqlite_open("./database.sqlite")
  if (dbh == 0)
    print("Failed to open ./database.sqlite; create it first (sqlite3 ./database.sqlite < ./examples/data/database.sql)")
    return 1

  lfd = tcp_listen(port, 16)
  if (lfd == 0)
    print("Failed to listen on port " + to_string(port))
    return 1

  print("sqlited: listening on " + host + " " + to_string(port))
  while (true)
    cfd = tcp_accept(lfd)
    if (cfd > 0)
      // Handle sequentially to keep it simple for a demo
      handle_client(cfd, dbh)

// Explicitly invoke main when the script is run
main()
