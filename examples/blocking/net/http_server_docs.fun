#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-04-21
 */

#include <net/http_server.fun>

port = 8080
htdocs = "./web/_site/"

server = HTTPServer(port)
server.set_htdocs(htdocs)

server.start()
