---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - unix_socket_echo.fun — overview
subtitle: Documentation for unix_socket_echo.fun — overview
description: Documentation for unix_socket_echo.fun — overview
permalink: /documentation/examples/unix_socket_echo/
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

# unix_socket_echo.fun — overview

What it shows
- Local domain (UNIX) socket echo server/client demonstration.

How to run
- export FUN_LIB_DIR="./lib"
- ./build_debug/fun examples/unix_socket_echo.fun
- Or: fun examples/unix_socket_echo.fun

Notes
- Uses a UNIX domain socket path on the local filesystem; ensure you have permissions to create it.
