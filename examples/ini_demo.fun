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

// Minimal demo for INI opcodes using iniparser 4.2.6

path = "./examples/data/complex.ini"
h = ini_load(path)
if h == 0
  print("Failed to load " + path)
else
  u = ini_get_string(h, "auth", "user", "guest")
  r = ini_get_int(h, "network", "retries", 3)
  s = ini_get_bool(h, "network", "ssl", 0)
  print("user=" + u)
  print("retries=" + to_string(r))
  print("ssl=" + to_string(s))
  ok = ini_set(h, "auth", "token", "abcd1234")
  if ok
    ini_save(h, path)
  ini_free(h)

/* Expected output:
user=����U
retries=3
ssl=1
*/
