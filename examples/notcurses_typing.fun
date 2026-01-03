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

// BROKEN!!!

/*
 * Notcurses typing demo: shows last key code; ESC quits.
 */

include <ui/notcurses.fun>

n = Notcurses()
if n.init() == 0
  print("Notcurses not available. Rebuild with -DFUN_WITH_NOTCURSES=ON.")
  exit(0)

n.clear()
n.draw_text(1, 0, "Typing demo (ESC to quit)")

var running = 1
var last = -1
while running == 1
  // poll every 100ms
  var k = n.getch(100)
  if k != -1
    last = k
    if k == 27 // ESC
      running = 0
    // draw last key code
    n.draw_text(3, 0, "Last key code: " + to_string(last))

n.shutdown()
