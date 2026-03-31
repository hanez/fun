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

// Demonstration of INI subsections like [section.subsection]
// Uses iniparser 4.2.6 via Fun's ini_* opcodes

path = "./examples/data/subsections.ini"
h = ini_load(path)
if h == 0
  print("Failed to load "+path)
else
  // Top-level server
  srv_host = ini_get_string(h, "server", "host", "localhost")
  srv_port = ini_get_int(h, "server", "port", 80)

  // Subsection: server.tls
  tls_enabled = ini_get_bool(h, "server.tls", "enabled", 0)
  tls_version = ini_get_double(h, "server.tls", "version", 1.2)
  tls_ciphers = ini_get_string(h, "server.tls", "ciphers", "")

  // Subsections: users.*
  admin_name = ini_get_string(h, "users.admin", "name", "admin")
  admin_active = ini_get_bool(h, "users.admin", "active", 1)
  admin_quota = ini_get_int(h, "users.admin", "quota_gb", 10)

  guest_name = ini_get_string(h, "users.guest", "name", "guest")
  guest_active = ini_get_bool(h, "users.guest", "active", 0)
  guest_quota = ini_get_int(h, "users.guest", "quota_gb", 1)

  // Subsection: paths.logs
  logs_dir = ini_get_string(h, "paths.logs", "dir", "./logs")
  logs_rotate = ini_get_bool(h, "paths.logs", "rotate", 0)
  logs_max_files = ini_get_int(h, "paths.logs", "max_files", 5)

  // Print
  print("[server]")
  print("  host=" + srv_host)
  print("  port=" + to_string(srv_port))

  print("[server.tls]")
  print("  enabled=" + to_string(tls_enabled))
  print("  version=" + to_string(tls_version))
  print("  ciphers=" + tls_ciphers)

  print("[users.admin]")
  print("  name=" + admin_name)
  print("  active=" + to_string(admin_active))
  print("  quota_gb=" + to_string(admin_quota))

  print("[users.guest]")
  print("  name=" + guest_name)
  print("  active=" + to_string(guest_active))
  print("  quota_gb=" + to_string(guest_quota))

  print("[paths.logs]")
  print("  dir=" + logs_dir)
  print("  rotate=" + to_string(logs_rotate))
  print("  max_files=" + to_string(logs_max_files))

  ini_free(h)

/* Expected output:
[server]
  host=example.org
  port=8080
[server.tls]
  enabled=1
  version=1.3
  ciphers=TLS_AES_256_GCM_SHA384,TLS_CHACHA20_POLY1305_SHA256
[users.admin]
  name=alice
  active=1
  quota_gb=100
[users.guest]
  name=bob
  active=0
  quota_gb=5
[paths.logs]
  dir=./var/log/fun
  rotate=1
  max_files=7
*/

