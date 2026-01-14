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

/*
 * Minimal Notcurses hello example.
 */

include <ui/notcurses.fun>

n = Notcurses()

if n.init() == 0
  print("Notcurses not available. Rebuild with -DFUN_WITH_NOTCURSES=ON.")
  exit(0)

n.clear()
n.draw_text(2, 0, "Fun + Notcurses")
n.draw_text(4, 0, "Press any key to exit...")

// blocking until key
_ = n.getch(0)
n.shutdown()

/* Possible output:
A TUI.

After exit:
3 renders, 991,14µs (223,57µs min, 330,38µs avg, 531,91µs max)
3 rasters, 320,76µs (106,45µs min, 106,92µs avg, 107,72µs max)
3 writes, 198,26µs (62,54µs min, 66,09µs avg, 69,94µs max)
59B (0B min, 19B avg, 30B max) 1 input Ghpa: 0
0 failed renders, 0 failed rasters, 0 refreshes, 0 input errors
RGB emits:elides: def 1:38 fg 0:0 bg 0:0
Cell emits:elides: 39:74805 (99,95%) 97,44% 0,00% 0,00%
Bmap emits:elides: 0:0 (0,00%) 0B (0,00%) SuM: 0 (0,00%)
*/
