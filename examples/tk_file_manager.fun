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

print("Initializing Fun File Manager...")
tk = TK()
tk.title("Fun File Manager")

// Current directory
dir = env("PWD")
if (dir == "")
  dir = "."
print("Current directory: " + dir)

tk.label("path", "Current Dir: " + dir)
tk.pack("path")

// Files listbox
tk.listbox("files")
tk.pack("files")

// Populate listbox
fun refresh(tk, dir)
  print("Refreshing file list for: " + dir)
  tk.clear("files")
  files = os_list_dir(dir)
  print("Found " + to_string(len(files)) + " entries.")
  for f in files
    tk.insert("files", "end", f)

refresh(tk, dir)

// Refresh button
// Using tk.eval for button because tk.button currently exits the app.
tk.eval("button .refresh -text {Refresh} -command {puts {Refresh requested}}")
tk.pack("refresh")

// Exit button
tk.button("exit", "Exit")
tk.pack("exit")

print("Entering Tk loop...")
tk.loop()
print("Tk loop exited.")

/* Expected output:
A GUI... ;)
*/
