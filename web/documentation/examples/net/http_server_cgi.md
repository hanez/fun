---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - http_server_cgi.fun
subtitle: Documentation for http_server_cgi.fun
description: Documentation for http_server_cgi.fun
permalink: /documentation/examples/net/http_server_cgi/
lang: en
tags:
- example
- http_server_cgi
- network
---


- Location: examples/net/http_server_cgi.fun
- Category: Networking / HTTP with CGI (blocking)

Description
- Minimal CGI-capable HTTP server (blocking) using lib/net/http_cgi_server.fun. Serves static files and executes .fun scripts as CGI.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"   # optional; auto-detected if omitted
  - export FUN_HTDOCS="./examples/data/htdocs"  # optional
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Try: http://127.0.0.1:8080/ and /hello.fun, /info.fun

Requirements
- Uses stdlib io/socket.fun, strings.fun, and net/cgi.fun. No external extensions required.

See also
- documentation/examples/README.md
- documentation/examples/httpserver.md (deep-dive)
