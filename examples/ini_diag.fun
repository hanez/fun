#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-02
 */

// Minimal diagnostic for INI lookups

path = "./examples/data/complex.ini"
h = ini_load(path)
print("h=" + to_string(h))
if h == 0
  print("Failed to load: " + path)
else
  print("[try app:name]")
  v1 = ini_get_string(h, "app", "name", "<def>")
  print("app:name => " + v1)

  print("[try app:version]")
  v2 = ini_get_string(h, "app", "version", "<def>")
  print("app:version => " + v2)

  print("[try database:port]")
  v3 = ini_get_int(h, "database", "port", -1)
  print("database:port => " + to_string(v3))

  print("[try network:ssl]")
  v4 = ini_get_bool(h, "network", "ssl", -9)
  print("network:ssl => " + to_string(v4))

  ini_free(h)

/* Expected output:
h=1
[try app:name]
app:name => FunApp
[try app:version]
app:version => 1.2.3
[try database:port]
database:port => 5432
[try network:ssl]
network:ssl => 1
*/
