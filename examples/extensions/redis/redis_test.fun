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
 */

h = redis_connect('127.0.0.1', 6379)

print(typeof(h))

print(redis_cmd(h, 'PING'))

_ = redis_cmd(h, 'SET fun_demo_key 42')

print(redis_cmd(h, 'GET fun_demo_key'))

redis_close(h)
