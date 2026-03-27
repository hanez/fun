# info.fun (CGI)

- Location: examples/data/htdocs/info.fun
- Category: CGI script used by HTTP server examples

Description
- CGI script that prints request/environment information, useful for debugging CGI variables.

How to run
- Through one of the HTTP server examples, e.g.:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Open: http://127.0.0.1:8080/info.fun

See also
- docs/examples/httpserver.md (deep-dive)
