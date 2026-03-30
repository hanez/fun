#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-23
 */

#include <ui/tk.fun>

print("Testing os_list_dir...")
files = os_list_dir(".")
print("Found " + to_string(len(files)) + " files.")
if len(files) > 0
  print("First file: " + files[0])

print("Testing tk_bind parsing...")
// We can't easily test Tk without an X server, but we can see if it crashes.
// If built with FUN_WITH_TCLTK, it should at least initialize.
// We use tk_eval to avoid full loop.
rc = tk_eval("set x 1")
print("tk_eval rc: " + to_string(rc))
if rc == 0
  print("Tcl Result: " + tk_result())

/* Expected output:
Testing os_list_dir...
Found 23 files.
First file: build
Testing tk_bind parsing...
tk_eval rc: 0
Tcl Result: 1
*/
