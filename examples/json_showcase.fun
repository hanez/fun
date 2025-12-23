#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-24
 */

// Demonstrates JSON.parse/stringify/from_file/to_file via the stdlib JSON class.

include <io/json.fun>

json = JSON()

print("-- JSON: parse from string and pretty print --")

// Build a sample JSON text with most value types
sample = '{"name":"Ada","active":true,"score":99.5,"count":42,"tags":["C","Ada","Math"],"extra":null}'
obj = json.parse(sample)
print("Dump:")
print(obj)
print(obj["name"])         // Ada
print(obj["active"])       // 1
print(obj["count"])        // 42
print(len(obj["tags"]))    // 3

pretty = json.stringify(obj, 1)
print(pretty)

print("-- JSON: load from file, inspect, and save pretty to /tmp --")

// Load non existent json file
path = "examples/data/nonexistent.json"
cfg = json.from_file(path)
print("Dump:")
print(cfg)

// Load a more complex example shipped with the repo
path = "examples/data/complex.json"
cfg = json.from_file(path)
print("Dump:")
print(cfg)

// Access nested fields
print(cfg["project"]["name"])         // project name
print(cfg["project"]["version"])      // version string
print(len(cfg["users"]))               // number of users

// Derive a small summary map
summary = {}
summary["user_count"] = len(cfg["users"])
summary["first_user_name"] = cfg["users"][1]["name"]
summary["features_enabled"] = cfg["features"]["enabled"]

print(json.stringify(summary, 1))

// Write the loaded config back as pretty JSON
out_path = "/tmp/fun_complex_out.json"
ok = json.to_file(out_path, cfg, 1)
print(ok) // 1 on success

/* Expected output:
-- JSON: parse from string and pretty print --
Dump:
{"name": Ada, "active": true, "score": 99.5, "count": 42, "tags": [C, Ada, Math], "extra": nil}
Ada
true
42
3
{
  "name":"Ada",
  "active":true,
  "score":99.5,
  "count":42,
  "tags":[
    "C",
    "Ada",
    "Math"
  ],
  "extra":null
}
-- JSON: load from file, inspect, and save pretty to /tmp --
Dump:
nil
Dump:
{"project": {"name": Fun, "version": 0.27.2, "website": https://fun-lang.xyz, "license": {"name": Apache-2.0, "url": https://opensource.org/license/apache-2-0}}, "features": {"enabled": [arrays, maps, json, pcsc], "experimental": {"repl": true, "sockets": true, "odbc": false, "notes": nil}}, "users": [{"id": 1, "name": Ada, "roles": [admin, math], "active": true, "score": 99.5, "prefs": {"theme": dark, "editor": {"tabWidth": 2, "font": Fira Code}}}, {"id": 2, "name": Linus, "roles": [user, kernel], "active": false, "score": 88, "prefs": {"theme": light, "editor": {"tabWidth": 8, "font": Monospace}}}], "metrics": {"counters": [0, 1, 1, 2, 3, 5, 8], "latency_ms": {"p50": 1.23, "p90": 3.21, "p99": 12.34}, "builds": 1234567890123456789, "last_release_ts": 1732406400000}, "matrix": [[1, 2, 3], [4, 5, 6], [7, 8, 9]], "notes": UTF-8 ✓ – emojis: 🚀🔥, "null_field": nil}
Fun
0.27.2
2
{
  "user_count":2,
  "first_user_name":"Linus",
  "features_enabled":[
    "arrays",
    "maps",
    "json",
    "pcsc"
  ]
}
1
*/

