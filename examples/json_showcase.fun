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
