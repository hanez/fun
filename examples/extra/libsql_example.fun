#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-26
 */

// Demonstrates the optional libSQL extension
// Build with: cmake -S . -B build -DFUN_WITH_LIBSQL=ON && cmake --build build

// Prepare sample DB from SQL if needed (requires sqlite3 CLI installed)
// Create it once with:
// sqlite3 ./database.sqlite < ./examples/data/database.sql

number h = libsql_open("./database.sqlite")
if h == 0
  print("Failed to open libSQL database")
else
  libsql_exec(h, "CREATE TABLE IF NOT EXISTS todos(id INTEGER PRIMARY KEY, title TEXT, done INT)")
  libsql_exec(h, "DELETE FROM todos")
  libsql_exec(h, "INSERT INTO todos(title, done) VALUES('Buy milk', 0)")
  libsql_exec(h, "INSERT INTO todos(title, done) VALUES('Write code', 1)")

  rows = libsql_query(h, "SELECT id, title, done FROM todos ORDER BY id")
  for row in rows
    print(to_string(row["id"]) + ": " + to_string(row["title"]) + " (done="+to_string(row["done"]) + ")")

  rows = libsql_query(h, "SELECT id, title, done, created_at FROM tasks ORDER BY id;")
  print("Tasks (" + to_string(len(rows)) + "):")
  for row in rows
    string status = "✘"
    if row["done"] == 1
      status = "✔"
    print("- [" + status + "] (#" + to_string(row["id"]) + ") " + to_string(row["title"]) + " — " + to_string(row["created_at"]))

  number rc = libsql_exec(h, "INSERT INTO tasks (title, done) VALUES ('Try Fun + SQLite', 0);")
  print("Insert rc=" + to_string(rc))

  rows2 = libsql_query(h, "SELECT count(*) AS cnt FROM tasks;")
  print("Total tasks now: " + to_string(rows2[0]["cnt"]))

  libsql_close(h)

/* Example output:
1: Buy milk (done=0)
2: Write code (done=1)
Tasks (3):
- [✔] (#1) Write Fun + SQLite example — 2025-11-26 23:20:41
- [✘] (#2) Ship optional feature flag — 2025-11-26 23:20:41
- [✘] (#3) Celebrate with coffee — 2025-11-26 23:20:41
Insert rc=0
Total tasks now: 4
*/
