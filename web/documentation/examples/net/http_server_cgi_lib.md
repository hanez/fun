---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - http_server_cgi_lib.fun
subtitle: Documentation for http_server_cgi_lib.fun
description: Documentation for http_server_cgi_lib.fun
permalink: /documentation/examples/net/http_server_cgi_lib/
lang: en
tags:
- example
- http_server_cgi_lib
- network
---


- Location: examples/net/http_server_cgi_lib.fun
- Category: Networking / HTTP with CGI (blocking, stdlib helpers)

Description
- Blocking HTTP server that serves static files and runs .fun as CGI using helpers from lib/net/http_cgi_lib_server.fun and net/cgi.fun.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"   # optional; auto-detected if omitted
  - export FUN_HTDOCS="./examples/data/htdocs"  # optional
  - ./build_debug/fun examples/net/http_server_cgi_lib.fun
  - Try: http://127.0.0.1:8080/ and /hello.fun, /info.fun

Requirements
- Uses stdlib io/socket.fun, strings.fun, net/cgi.fun. No external extensions required.

See also
- documentation/examples/README.md
- documentation/examples/httpserver.md (deep-dive)
