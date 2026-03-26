#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

/*
 * Minimal blocking HTTP server that serves static files and
 * handles a built-in CGI endpoint using the net/cgi.fun library.
 *
 * Try:
 *   - http://127.0.0.1:8080/
 *   - http://127.0.0.1:8080/hello.fun?name=Fun
  +  - http://127.0.0.1:8080/info.fun?name=Fun
 */

#include <net/http_cgi_lib_server.fun>

port = 8080
htdocs = "./examples/data/htdocs"

server = HTTPCGILibServer(port)
server.set_htdocs(htdocs)
server.start()
