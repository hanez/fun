#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-12
 */

// Realtime progress bar demo without printing extra newlines.
// It only updates a single console line in-place. The progress()
// helper emits one trailing newline automatically at 100%.

// includes can also be done with a leading # like in C, but some programmers maybe like more clean code without a
// leading #.
include <io/console.fun>
include <utils/datetime.fun>  // for sleep()

c = Console()

print("Progress demo: 0..100")

total = 100
for i in range(0, total + 1)
  c.progress(i, total, "Downloading")
  sleep(30)  // milliseconds

print("\nMultiple phases demo")

// Phase 1
phase_total = 40
for i in range(0, phase_total + 1)
  c.progress(i, phase_total, "Phase 1")
  sleep(20)

// Phase 2
phase_total = 60
for i in range(0, phase_total + 1)
  c.progress(i, phase_total, "Phase 2")
  sleep(15)

/* Expected output:
Progress demo: 0..100
Downloading [=============================================================] 100%

Multiple phases demo
Phase 1 [=================================================================] 100%
Phase 2 [=================================================================] 100%
*/
