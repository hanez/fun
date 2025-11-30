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

// INI stdlib abstraction wrapping the ini_* VM builtins (iniparser 4.2.6).
//
// Usage:
//   include <io/ini.fun>
//   ini = INI()
//   if (ini.load("./examples/data/complex.ini") > 0)
//     name = ini.get_string("app", "name", "")
//     retries = ini.get_int("network", "retries", 0)
//     ini.set("app", "debug", 1)
//     ini.save(nil)  // save back to original path
//     ini.close()

class INI()
  // current handle (>0 when open) and path string
  h = 0
  path = ""

  // Load an INI file from path, closing previous one if open.
  // Returns handle (>0) or 0 on error.
  fun load(this, path)
    if (this.h > 0)
      ini_free(this.h)
      this.h = 0
    p = to_string(path)
    this.path = p
    this.h = ini_load(p)
    return this.h

  // True if a dictionary is open.
  fun is_open(this)
    return this.h > 0

  // Close and free resources. Safe to call multiple times.
  fun close(this)
    if (this.h > 0)
      ini_free(this.h)
      this.h = 0
    return 1

  // Getters with defaults. When not open, return the default converted.
  fun get_string(this, section, key, def)
    if (!this.is_open())
      return to_string(def)
    return ini_get_string(this.h, to_string(section), to_string(key), to_string(def))

  fun get_int(this, section, key, def)
    if (!this.is_open())
      return to_number(def)
    return ini_get_int(this.h, to_string(section), to_string(key), to_number(def))

  fun get_double(this, section, key, def)
    if (!this.is_open())
      return to_number(def)
    return ini_get_double(this.h, to_string(section), to_string(key), to_number(def))

  fun get_bool(this, section, key, def)
    if (!this.is_open())
      if (def == nil)
        return 0
      // treat 0/1 and boolean-like strings
      return to_number(def) != 0
    return ini_get_bool(this.h, to_string(section), to_string(key), to_number(def))

  // Set/unset return 1 on success, 0 on failure.
  fun set(this, section, key, value)
    if (!this.is_open())
      return 0
    return ini_set(this.h, to_string(section), to_string(key), to_string(value))

  fun unset(this, section, key)
    if (!this.is_open())
      return 0
    return ini_unset(this.h, to_string(section), to_string(key))

  // Save to a path. If path is nil, save to the last loaded path.
  fun save(this, path)
    if (!this.is_open())
      return 0
    p = path
    if (p == nil)
      p = this.path
    p = to_string(p)
    if (len(p) == 0)
      return 0
    return ini_save(this.h, p)
