#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-24
 */

// JSON stdlib abstraction wrapping VM json_* builtins defensively.

class JSON()
  fun parse(this, text)
    v = json_parse(to_string(text))
    return v

  fun stringify(this, value, pretty)
    if pretty == nil
      pretty = 0
    return json_stringify(value, pretty)

  fun from_file(this, path)
    return json_from_file(to_string(path))

  fun to_file(this, path, value, pretty)
    if pretty == nil
      pretty = 0
    return json_to_file(to_string(path), value, pretty)
