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

  // --- Extended primitives ---
  fun size(this)
    return nc_get_size()

  fun render(this)
    return nc_render()

  // fg/bg are 0xRRGGBB, style bitmask: bold(1), dim(2), italic(4), underline(8), strike(16), reverse(32)
  fun set_style(this, fg, bg, style)
    // parser order for builtin is (style,bg,fg)
    return nc_set_style(to_number(style), to_number(bg), to_number(fg))

  fun draw_char(this, y, x, ch)
    return nc_draw_char(to_number(y), to_number(x), to_number(ch))

  fun hline(this, y, x, len, ch)
    return nc_draw_hline(to_number(y), to_number(x), to_number(len), to_number(ch))

  fun vline(this, y, x, len, ch)
    return nc_draw_vline(to_number(y), to_number(x), to_number(len), to_number(ch))

  fun box(this, y, x, h, w, style)
    // builtin expects (x,y,w,h,style)
    return nc_box(to_number(x), to_number(y), to_number(w), to_number(h), to_number(style))

  fun fill(this, y, x, h, w, ch)
    // builtin expects (x,y,w,h,ch)
    return nc_fill(to_number(x), to_number(y), to_number(w), to_number(h), to_number(ch))

  // --- Widgets as methods ---
  // Status bar on the last row
  fun status_bar(this, text, style)
    sz = this.size()
    if style == nil
      style = 32
    rc = this.set_style(0x000000, 0xFFFFFF, style)
    rc = this.fill(sz[0] - 1, 0, 1, sz[1], 32)
    rc = this.draw_text(sz[0] - 1, 1, text)
    rc = this.render()
    return 0

  // Draw styled label
  fun label(this, y, x, text, style)
    if style != nil
      rc = this.set_style(0xFFFFFF, 0x000000, style)
    rc = this.draw_text(y, x, text)
    rc = this.render()
    return 0

  // Panel with optional title
  fun box_panel(this, y, x, h, w, title, style)
    if style == nil
      style = 0
    rc = this.box(y, x, h, w, style)
    if title != nil
      rc = this.draw_text(y, x + 2, to_string(title))
    rc = this.render()
    return 0

  // Progress bar (frac in 0..1)
  fun progress(this, y, x, w, frac, style)
    if style == nil
      style = 0
    if frac < 0
      frac = 0
    if frac > 1
      frac = 1
    // frame and title
    rc = nc_box(to_number(x - 2), to_number(y - 1), to_number(w + 4), 3, to_number(style))
    // background in dim gray
    rc = this.set_style(0x606060, 0x000000, 2)
    rc = nc_fill(to_number(y), to_number(x), to_number(w), 1, 46) // '.'
    // filled portion in bold green
    rc = this.set_style(0x00FF00, 0x000000, 1)
    fw = to_number(frac) * w
    if fw < 0
      fw = 0
    if fw > w
      fw = w
    if fw > 0
      rc = nc_fill(to_number(y), to_number(x), to_number(fw), 1, 35) // '#'
    rc = nc_draw_text(to_number(y - 1), to_number(x), "Progress")
    rc = nc_render()
    return 0

  // Simple vertical menu, highlight selected index
  fun menu(this, y, x, items, selected_idx, w, style, sel_style)
    if style == nil
      style = 0
    if sel_style == nil
      sel_style = 32
    i = 0
    while i < len(items)
      if i == selected_idx
        rc = this.set_style(0x000000, 0xFFFFFF, sel_style)
      else
        rc = this.set_style(0xFFFFFF, 0x000000, style)
      if w != nil
        if len(to_string(items[i])) > w
          rc = this.draw_text(y + i, x, substr(to_string(items[i]), 0, w))
        else
          rc = this.draw_text(y + i, x, to_string(items[i]))
      else
        rc = this.draw_text(y + i, x, to_string(items[i]))
      i = i + 1
    rc = this.render()
    return 0

  // Input line echo with cursor
  fun input_line(this, y, x, prompt, buffer, cursor_idx, w, style)
    if style == nil
      style = 0
    if w == nil
      w = len(buffer) + len(prompt) + 2
    rc = this.set_style(0xFFFFFF, 0x000000, style)
    rc = this.draw_text(y, x, to_string(prompt))
    rc = this.fill(y, x + len(prompt), 1, w, 32)
    rc = this.draw_text(y, x + len(prompt), to_string(buffer))
    this.set_style(0x000000, 0xFFFFFF, 32)
    ci = cursor_idx
    if ci < 0
      ci = 0
    if ci > len(buffer)
      ci = len(buffer)
    rc = this.draw_char(y, x + len(prompt) + ci, 32)
    rc = this.render()
    return 0

// --- End of class Notcurses ---
