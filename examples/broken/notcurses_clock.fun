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
 * Notcurses clock demo: updates time every 200ms; any key to finish.
 */

include <ui/notcurses.fun>

n = Notcurses()
if n.init() == 0
  print("Notcurses not available. Rebuild with -DFUN_WITH_NOTCURSES=ON.")
  exit(0)

n.clear()
n.draw_text(1, 0, "Clock demo (press any key)")

var done = 0
while done == 0
  // current time string
  var ms = time_now_ms()
  var t = date_format("%Y-%m-%d %H:%M:%S", ms)
  n.draw_text(3, 0, "Time: " + t)

  // poll without blocking too long
  var k = n.getch(200)
  if k != -1
    done = 1

n.shutdown()
