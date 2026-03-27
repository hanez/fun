# http_server_cgi.fun

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
- docs/examples/README.md
- docs/examples/httpserver.md (deep-dive)
