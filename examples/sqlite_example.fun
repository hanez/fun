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

// Prepare sample DB from SQL if needed (requires sqlite3 CLI installed)
// Create it once with:
// sqlite3 todo.sqlite < ./examples/data/todo.sql

string db_path = "./todo.sqlite"

number h = sqlite_open(db_path)
if h == 0
  print("Failed to open DB: " + db_path)
  exit(1)

rows = sqlite_query(h, "SELECT id, title, done, created_at FROM tasks ORDER BY id;")
print("Tasks (" + to_string(len(rows)) + "):")
for row in rows
  string status = "✘"
  if row["done"] == 1
    status = "✔"
  print("- [" + status + "] (#" + to_string(row["id"]) + ") " + to_string(row["title"]) + " — " + to_string(row["created_at"]))

number rc = sqlite_exec(h, "INSERT INTO tasks (title, done) VALUES ('Try Fun + SQLite', 0);")
print("Insert rc=" + to_string(rc))

rows2 = sqlite_query(h, "SELECT count(*) AS cnt FROM tasks;")
print("Total tasks now: " + to_string(rows2[0]["cnt"]))

sqlite_close(h)
