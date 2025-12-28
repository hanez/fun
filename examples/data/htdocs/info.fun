#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

result = proc_run("fun -V")
content = result["out"]

print("<html><body>")
print("<h1>Fun Environment Information</h1>")
print("<p><strong>Current Path:</strong> " + env("PATH") + "</p>")
print("<p><strong>Current User:</strong> " + env("USER") + "</p>")
print("<p><strong>Time:</strong> " + date_format(time_now_ms(), "%Y-%m-%d %H:%M:%S") + "</p>")
print("<p><strong>Version:</strong> " + to_string(content) + "</p>")
print("</body></html>")
