#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

cmd = "./build_debug/fun ./examples/basics/hello_world.fun"
res = proc_run(cmd)
print("exit: " + to_string(res["code"]))
print("out:  " + to_string(res["out"]))
print("err:  " + to_string(res["err"]))

/* Expected output (with build_debug/fun present):
exit: 0
out:  Hello, World!\n

err: nil
*/
