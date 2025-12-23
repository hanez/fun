#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

// Demonstrates the Tk stdlib wrapper class using the new Tk opcodes.

include <ui/tk.fun>

tk = TK()

tk.title("Fun + Tk GUI")

tk.label("hello", "Hello, world!")
tk.pack("hello")

tk.button("ok", "OK")
tk.pack("ok")

// Enter GUI loop (no-op if built without FUN_WITH_TCLTK)
tk.loop()

/* Expected output:
A GUI... ;)
*/

