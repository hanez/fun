#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-03
 */

// Notcurses stdlib abstraction wrapping VM NC_* builtins.
// Provides a tiny OO-style helper around nc_init/nc_clear/nc_draw_text/nc_getch/nc_shutdown.
// If Fun was built without Notcurses support, init() returns 0 and methods are no-ops.

class Notcurses()
  // Initialize Notcurses core. Returns 1 on success, 0 on failure.
  fun init(this)
    return nc_init()

  // Clear the screen and render. Returns 0 on success, -1 on error.
  fun clear(this)
    return nc_clear()

  // Draw text at y, x and render. Returns 0 on success, -1 on error.
  fun draw_text(this, y, x, text)
    // allow flexible types
    return nc_draw_text(to_number(y), to_number(x), to_string(text))

  // Get a Unicode codepoint (int). timeout_ms<=0 blocks; >0 returns -1 if no key ready.
  fun getch(this, timeout_ms)
    if timeout_ms == nil
      timeout_ms = 0
    return nc_getch(to_number(timeout_ms))

  // Shut down Notcurses, restoring the terminal.
  fun shutdown(this)
    return nc_shutdown()

  // (banner helper removed temporarily due to parser limitations)
