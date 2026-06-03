#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-06-03
 */

/*
 * Simple Redis test using Fun builtins backed by hiredis.
 * 
 * Hash operations (HSET/HGET/HGETALL)
 */

h = redis_connect('127.0.0.1', 6379)
key = 'fun:examples:redis:hash:user1'

// Start fresh
_ = redis_cmd(h, 'DEL ' + key)

// Set a couple of fields
print(redis_cmd(h, 'HSET ' + key + ' name Alice'))
print(redis_cmd(h, 'HSET ' + key + ' age 30'))

// Fetch a single field
print('HGET name -> ' + redis_cmd(h, 'HGET ' + key + ' name'))

// Fetch all fields (returns a flat array [field, value, field, value, ...])
all = redis_cmd(h, 'HGETALL ' + key)
print('HGETALL ->')
print(all)

redis_close(h)
