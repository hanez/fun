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

#include <strings.fun>

res1 = proc_run("echo one")
res2 = proc_run("echo two")
out = str_trim(res1["out"]) + "," + str_trim(res2["out"]) 
print(out)

/* Expected output:
one,two
*/
