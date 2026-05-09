/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

// Thin wrapper around VM kcgi intrinsics

class KCGI()
  fun _construct(this)
    this.req = kcgi_parse()

  fun get(this, key)
    if (typeof(this.req) == "Map")
      fields = this.req["fields"]
      if (typeof(fields) == "Map")
        return fields[key]
    return nil

  // Returns the field value if present and non-nil, otherwise returns 'def'.
  fun get_or(this, key, def)
    v = this.get(key)
    if (v == nil)
      return def
    // Some code may stringify nil to "nil"; treat that as missing too.
    if (typeof(v) == "String" && v == "nil")
      return def
    return v

  fun reply(this, code, content_type, body)
    if (kcgi_reply_start(code, content_type) == 1)
      _ = kcgi_write(to_string(body))
    _ = kcgi_end()

  // streaming API
  fun start(this, code, content_type)
    return kcgi_reply_start(code, content_type)

  fun write(this, chunk)
    return kcgi_write(to_string(chunk))

  fun end(this)
    return kcgi_end()
