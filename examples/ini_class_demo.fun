#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-30
 */

// Demonstration of the Ini stdlib class from lib/io/ini.fun
include <io/ini.fun>

ini = INI()
path = "./examples/data/complex.ini"

if (ini.load(path) == 0)
  print("Failed to load " + path)
  exit(1)

// Read a few values
app_name = ini.get_string("app", "name", "FunApp")
app_version = ini.get_string("app", "version", "0.0.0")
app_debug = ini.get_bool("app", "debug", 0)

db_host = ini.get_string("database", "host", "localhost")
db_port = ini.get_int("database", "port", 5432)

print("[app]")
print("  name=" + app_name)
print("  version=" + app_version)
print("  debug=" + to_string(app_debug))

print("[database]")
print("  host=" + db_host)
print("  port=" + to_string(db_port))

// Update a value and save back to the same file
ini.set("app", "debug", 1)
ok = ini.save(nil)
print("saved=" + to_string(ok))

ini.close()
