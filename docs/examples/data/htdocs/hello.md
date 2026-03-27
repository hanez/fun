# hello.fun (CGI)

- Location: examples/data/htdocs/hello.fun
- Category: CGI script used by HTTP server examples

Description
- Simple CGI .fun script that prints a greeting, optionally using query parameters (e.g., ?name=Fun).

How to run
- Through one of the HTTP server examples, e.g.:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Open: http://127.0.0.1:8080/hello.fun?name=Fun

See also
- docs/examples/httpserver.md (deep-dive)
