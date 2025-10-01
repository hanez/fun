/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/*
 * Process utilities built on proc_run() and system().
 *
 * Methods:
 *   run(cmd) -> map { "out": string, "code": number }
 *   run_merge_stderr(cmd) -> same, but merges stderr into stdout (" 2>&1")
 *   system(cmd) -> exit code number
 *   check_call(cmd) -> 1 if exit code==0 else 0
 */

class Process()
  // Execute command and capture stdout and exit code
  fun run(this, cmd)
    return proc_run(to_string(cmd))

  // Merge stderr into stdout using shell redirection
  fun run_merge_stderr(this, cmd)
    c = to_string(cmd)
    // Add a single space if needed
    if (len(c) > 0) && (substr(c, len(c) - 1, 1) != " ")
      c = join([c, " 2>&1"], "")
    else
      c = join([c, "2>&1"], "")
    return proc_run(c)

  // Return the exit code of the command
  fun system(this, cmd)
    return system(to_string(cmd))

  // Return 1 if command succeeds (exit code 0), else 0
  fun check_call(this, cmd)
    code = system(to_string(cmd))
    if (code == 0)
      return 1
    else
      return 0
