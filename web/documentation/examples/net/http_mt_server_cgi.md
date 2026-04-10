---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - http_mt_server_cgi.fun
subtitle: Documentation for http_mt_server_cgi.fun
description: Documentation for http_mt_server_cgi.fun
permalink: /documentation/examples/net/http_mt_server_cgi/
lang: en
tags:
- documentation
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
- docs
- specifications
- specs
- repl
---

# http_mt_server_cgi.fun

- Location: examples/net/http_mt_server_cgi.fun
- Category: Networking / HTTP with CGI (multi-threaded)

Description
- Multi-threaded HTTP server (thread-per-connection) with CGI support. Serves static files from htdocs and executes .fun scripts as CGI using net/cgi.fun helpers.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"   # optional; auto-detected if omitted
  - export FUN_HTDOCS="./examples/data/htdocs"  # optional
  - ./build_debug/fun examples/net/http_mt_server_cgi.fun
  - Try: http://127.0.0.1:8080/ and /hello.fun?name=Fun, /info.fun

Requirements
- Uses stdlib io/socket.fun, io/thread.fun, and net/cgi.fun. No external extensions required.

See also
- documentation/examples/README.md
- documentation/examples/httpserver.md (deep-dive)
