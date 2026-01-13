#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-13
 */

// Realtime progress bar demo without printing extra newlines.
// It only updates a single console line in-place. The progress()
// helper emits one trailing newline automatically at 100%.

include <io/console.fun>
include <utils/datetime.fun>  // for sleep()

c = Console()

total = 100
for i in range(0, total + 1)
  c.progress(i, total, "Downloading")
  sleep(20)  // milliseconds

// Done. No additional prints/newlines here; progress() already
// finalized the line when it reached 100%.

/* Expected output:
Downloading [=============================================================] 100%
*/
