# counter.fun (CGI)

- Location: examples/data/htdocs/counter.fun
- Category: CGI script used by HTTP server examples

Description
- Simple stateful counter example for CGI; demonstrates reading and updating a value across requests (implementation details in script).

How to run
- Through one of the HTTP server examples, e.g.:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Open: http://127.0.0.1:8080/counter.fun

See also
- docs/examples/httpserver.md (deep-dive)
