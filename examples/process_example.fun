#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/*
 * Subprocess example using proc_run/system and the Process class.
 */

#include <io/process.fun>

print("=== Built-ins ===")
r = proc_run("echo Hello from Fun")
print(r["out"])
print(join(["exit code: ", to_string(r["code"])], ""))

code = system("sh -c 'exit 3'")
print(join(["system() exit code: ", to_string(code)], ""))

print("=== Process class ===")
p = Process()
res = p.run("uname -s")
print(join(["OS: ", res["out"]], ""))

res2 = p.run_merge_stderr("sh -c 'echo out; echo err 1>&2; exit 1'")
print("merged output:")
print(res2["out"])
print(join(["exit code: ", to_string(res2["code"])], ""))

ok = p.check_call("true")
print(join(["check_call(true): ", to_string(ok)], ""))

ok2 = p.check_call("false")
print(join(["check_call(false): ", to_string(ok2)], ""))

/* Expected output:
=== Built-ins ===
Hello from Fun

exit code: 0
system() exit code: 3
=== Process class ===
OS: Linux

merged output:
out
err

exit code: 1
check_call(true): 1
check_call(false): 0
*/
