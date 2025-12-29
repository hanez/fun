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

#include <strings.fun>

print("<!doctype html><html><body>")
print("<h1>Fun Environment Information</h1>")
print("<p><strong>Current Path:</strong> " + env("PATH") + "</p>")
print("<p><strong>Current User:</strong> " + env("USER") + "</p>")
print("<p><strong>Time:</strong> " + date_format(time_now_ms(), "%Y-%m-%d %H:%M:%S") + "</p>")
print("<p><strong>Local Version:</strong> " + to_string(fun_version()) + "</p>")
res = proc_run("fun -V")
installed_version = res["out"]
print("<p><strong>Installed Version:</strong> " + to_string(substr(installed_version, 4, len(installed_version)-2)) + "</p>")
print("<h2>Full Environment:</h2>")
print("<ul>")
all_env = env_all()
all_keys = keys(all_env)
i = 0
while i < len(all_keys)
    key = all_keys[i]
    print("<li>" + key + "=" + all_env[key] + "</li>")
    i = i + 1
print("</ul>")
print("<h2>GET Variables:</h2>")
print("<ul>")
qs = env("QUERY_STRING")
if (len(qs) > 0)
    params = str_split(qs, "&")
    i = 0
    while i < len(params)
        kv = str_split(params[i], "=")
        if (len(kv) == 2)
            print("<li>" + kv[0] + " = " + kv[1] + "</li>")
        else
            print("<li>" + params[i] + "</li>")
        i = i + 1
print("</ul>")
print("<h2>POST Variables:</h2>")
print("<ul>")
pd = env("POST_DATA")
if (len(pd) > 0)
    params = str_split(pd, "&")
    i = 0
    while i < len(params)
        kv = str_split(params[i], "=")
        if (len(kv) == 2)
            print("<li>" + kv[0] + " = " + kv[1] + "</li>")
        else
            print("<li>" + params[i] + "</li>")
        i = i + 1
print("</ul>")
print("</body></html>")
