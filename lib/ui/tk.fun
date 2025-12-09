#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

// Tk stdlib helper wrapping the Tk VM builtins.
// No raw Tcl is exposed; this class provides a tiny, safe GUI surface.

class TK()
  // Set the window title
  fun title(this, title)
    return tk_title(to_string(title))

  // Create or update a label widget with id and text
  fun label(this, id, text)
    return tk_label(to_string(id), to_string(text))

  // Create or update a button widget with id and text
  fun button(this, id, text)
    return tk_button(to_string(id), to_string(text))

  // Pack a widget by id
  fun pack(this, id)
    return tk_pack(to_string(id))

  // Enter the Tk event loop (blocks until windows are closed)
  fun loop(this)
    return tk_loop()
