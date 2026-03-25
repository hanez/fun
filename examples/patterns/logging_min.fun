#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Minimal helper: remove trailing newline from a string (no full strings lib needed)
fun chomp(s)
  t = to_string(s)
  if (len(t) > 0 && substr(t, len(t) - 1, 1) == "\n")
    return substr(t, 0, len(t) - 1)
  return t

fun now()
  // Very rough timestamp using system 'date' for demo
  r = proc_run("date +%Y-%m-%dT%H:%M:%S%z")
  return chomp(r["out"])

fun log_line(level, msg)
  print("[" + now() + "] [" + to_string(level) + "] " + to_string(msg))

log_line("INFO", "Service starting")
log_line("DEBUG", "Config loaded")
log_line("INFO", "Service running")

/* Expected output (timestamps will vary):
[2026-03-25T23:00:00+0000] [INFO] Service starting
[2026-03-25T23:00:00+0000] [DEBUG] Config loaded
[2026-03-25T23:00:00+0000] [INFO] Service running
*/
