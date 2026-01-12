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

  fun progress(this, length)
    return length
