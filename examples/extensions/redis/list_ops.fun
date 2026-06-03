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
 * List operations (LPUSH/LRANGE)
 */

h = redis_connect('127.0.0.1', 6379)
key = 'fun:examples:redis:list'

// Start fresh
_ = redis_cmd(h, 'DEL ' + key)

// Push some values to the left
print(redis_cmd(h, 'LPUSH ' + key + ' a'))
print(redis_cmd(h, 'LPUSH ' + key + ' b'))
print(redis_cmd(h, 'LPUSH ' + key + ' c'))

// Read entire list
vals = redis_cmd(h, 'LRANGE ' + key + ' 0 -1')
print('LRANGE 0 -1 -> ')
print(vals)

redis_close(h)
