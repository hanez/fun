#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-30
 */

// Complex INI parsing example using iniparser 4.2.6 opcodes.

path = "./examples/data/complex.ini"
h = ini_load(path)
if h == 0
  print("Failed to load "+path)
else
  // app
  app_name = ini_get_string(h, "app", "name", "FunApp")
  app_version = ini_get_string(h, "app", "version", "0.0.0")
  app_debug = ini_get_bool(h, "app", "debug", 0)

  // database
  db_host = ini_get_string(h, "database", "host", "localhost")
  db_port = ini_get_int(h, "database", "port", 5432)
  db_user = ini_get_string(h, "database", "user", "user")
  db_pass = ini_get_string(h, "database", "pass", "")
  db_pool = ini_get_int(h, "database", "pool_size", 4)
  db_timeout = ini_get_double(h, "database", "timeout", 2.0)

  // network
  net_ssl = ini_get_bool(h, "network", "ssl", 0)
  net_retries = ini_get_int(h, "network", "retries", 3)
  base_url = ini_get_string(h, "network", "base_url", "")

  // features
  feature_x = ini_get_bool(h, "features", "feature_x", 0)
  feature_y = ini_get_bool(h, "features", "feature_y", 0)

  // paths
  data_dir = ini_get_string(h, "paths", "data_dir", "./data")
  log_file = ini_get_string(h, "paths", "log_file", "./logs/app.log")

  // Print a structured summary
  print("[app]")
  print("  name=" + app_name)
  print("  version=" + app_version)
  print("  debug=" + to_string(app_debug))

  print("[database]")
  print("  host=" + db_host)
  print("  port=" + to_string(db_port))
  print("  user=" + db_user)
  print("  pass=" + db_pass)
  print("  pool_size=" + to_string(db_pool))
  print("  timeout=" + to_string(db_timeout))

  print("[network]")
  print("  ssl=" + to_string(net_ssl))
  print("  retries=" + to_string(net_retries))
  print("  base_url=" + base_url)

  print("[features]")
  print("  feature_x=" + to_string(feature_x))
  print("  feature_y=" + to_string(feature_y))

  print("[paths]")
  print("  data_dir=" + data_dir)
  print("  log_file=" + log_file)

  // Clean up
  ini_free(h)

/* Expected output:
[app]
  name=FunApp
  version=1.2.3
  debug=1
[database]
  host=localhost
  port=5432
  user=fun
  pass=secret
  pool_size=8
  timeout=2.5
[network]
  ssl=1
  retries=3
  base_url=https://api.example.com
[features]
  feature_x=1
  feature_y=0
[paths]
  data_dir=./data
  log_file=./logs/app.log
*/
