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
 * Simple key/value set-get-delete
 */

h = redis_connect('127.0.0.1', 6379)
key = 'fun:examples:redis:key'

// Clean slate
_ = redis_cmd(h, 'DEL ' + key)

// Set and get
print(redis_cmd(h, 'SET ' + key + ' 42'))
print('GET -> ' + redis_cmd(h, 'GET ' + key))

// Check existence
print('EXISTS -> ' + to_string(redis_cmd(h, 'EXISTS ' + key)))

// Delete
print('DEL -> ' + to_string(redis_cmd(h, 'DEL ' + key)))
print('EXISTS(after DEL) -> ' + to_string(redis_cmd(h, 'EXISTS ' + key)))

redis_close(h)
