/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/*
 * Console utilities: prompt, ask, and yes/no helpers built on input().
 */

#include <strings.fun>
#include <io/process.fun>

class Console()
  // Print a prompt and read a line (no trailing newline)
  fun prompt(this, text)
    return input(to_string(text))

  // Ask a question with ": " suffix; returns the user's response string
  fun ask(this, question)
    q = to_string(question)
    if (len(q) == 0)
      return input("")
    else
      return input(join([q, ": "], ""))

  // Ask for a secret string (e.g., password) without echoing input
  // Returns the entered string. A trailing newline is not echoed back.
  fun ask_hidden(this, question)
    q = to_string(question)
    if (len(q) == 0)
      return input_hidden("")
    else
      return input_hidden(join([q, ": "], ""))

  // Ask a yes/no question; returns 1 for yes, 0 for no
  // Accepts: y, yes, n, no (case-insensitive). Keeps asking until valid.
  fun ask_yes_no(this, question)
    q = to_string(question)
    while true
      ans = input(join([q, " [y/n]: "], ""))
      a = str_to_lower(ans)
      if (a == "y" || a == "yes")
        return 1
      else if (a == "n" || a == "no")
        return 0
      // otherwise loop again

  // Internal: best-effort terminal columns via `tput cols` (fallback to 80)
  fun term_cols(this)
    // Use builtin proc_run directly to avoid any method dispatch quirks
    r = proc_run("tput cols")
    if (r["code"] == 0)
      s = str_trim(r["out"])   // trim trailing newline
      if (len(s) > 0)
        n = to_number(s)
        if (n > 0)
          return n
    // Fallback if tput not available or not a TTY
    return 80

  // Draw/update a full-width progress bar on a single console line.
  // - current: Number (0..total)
  // - total:   Number (>0)
  // - label:   Optional text shown left of the bar
  // Returns the integer percent [0..100]. Adds a newline automatically at 100%.
  fun progress(this, current, total, label)
    cur = to_number(current)
    tot = to_number(total)
    if (tot <= 0)
      tot = 1
    pct = to_number((cur * 100) / tot)
    if (pct < 0)
      pct = 0
    if (pct > 100)
      pct = 100

    cols = this.term_cols()

    lbl = to_string(label)
    if (len(lbl) > 0)
      prefix = lbl + " "
    else
      prefix = ""

    perc_str = to_string(pct) + "%"
    // We render: [====....] plus percent; keep the whole line within terminal width
    // Compute bar width: terminal cols minus prefix, brackets, space, and percent.
    fixed = len(prefix) + 2 + 1 + len(perc_str)   // [] + space + percent
    bar_w = cols - fixed
    if (bar_w < 10)
      bar_w = 10  // minimal bar width for visibility

    filled = to_number((bar_w * pct) / 100)
    if (filled < 0)
      filled = 0
    if (filled > bar_w)
      filled = bar_w

    bar = "[" + str_repeat("=", filled) + str_repeat(" ", bar_w - filled) + "]"
    line = prefix + bar + " " + perc_str

    // In-place update: CR + full-width content (we already fit to cols), no newline
    echo("\r" + line)

    // When done, end the line cleanly
    if (cur >= tot)
      print("")

    return pct
