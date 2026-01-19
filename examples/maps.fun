#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-19
 */

/*
 * Maps datatype — focused examples
 *
 * This script exclusively demonstrates the built-in maps datatype:
 * - Creating maps with literals and empty maps
 * - Reading and writing entries via [key]
 * - Checking for key existence with has(map, key)
 * - Getting keys(map) and values(map)
 * - Using nested maps
 */

// Create an empty map and add entries
user = {}
user["name"] = "Ada"
user["age"] = 37
print(user)                   // -> {"name": "Ada", "age": 37}

// Read an entry by key
print(user["name"])          // -> Ada

// Update an existing entry
user["age"] = 38
print(user["age"])           // -> 38

// Check if a key exists
print(has(user, "age"))      // -> 1 (true)
print(has(user, "email"))    // -> 0 (false)

// Keys and values (order may vary)
print(keys(user))             // -> [name, age]
print(values(user))           // -> ["Ada", 38]

// Nested maps
address = { "city": "London", "zip": "E1" }
user["address"] = address
print(user["address"])       // -> {"city": "London", "zip": "E1"}
print(user["address"]["city"]) // -> London

/* Expected output:
{"name": Ada, "age": 37}
Ada
38
1
0
[name, age]
[Ada, 38]
{"zip": E1, "city": London}
London
*/
