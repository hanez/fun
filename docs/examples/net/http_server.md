# http_server.fun

- Location: examples/net/http_server.fun
- Category: Networking / HTTP (blocking)

Description
- Blocking HTTP server that serves static files and executes .fun CGI scripts via lib/net/http_server.fun.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - ./build_debug/fun examples/net/http_server.fun
  - Default docroot: ./examples/data/htdocs
  - Visit: http://127.0.0.1:8080/

Notes
- For CGI .fun under htdocs, the server uses the Fun interpreter to execute them and forwards output as HTTP.

Requirements
- Uses io/socket.fun and strings.fun; no external extensions required.

See also
- docs/examples/README.md
- docs/examples/httpserver.md (deep-dive)
