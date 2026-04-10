---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - http_mt_server.fun
subtitle: Documentation for http_mt_server.fun
description: Documentation for http_mt_server.fun
permalink: /documentation/examples/net/http_mt_server/
lang: en
tags:
- example
- http_mt_server
- network
---


- Location: examples/net/http_mt_server.fun
- Category: Networking / HTTP (multi-threaded)

Description
- Thread-per-connection HTTP server built on io/socket.fun and io/thread.fun. For each accepted client, spawns a thread and returns a small HTML page.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - ./build_debug/fun examples/net/http_mt_server.fun
  - Then open http://127.0.0.1:8080/ (or the port printed on start)

Requirements
- Uses stdlib io/socket.fun and io/thread.fun. No external extensions required.

See also
- documentation/examples/README.md
- documentation/examples/httpserver.md (deep-dive)
