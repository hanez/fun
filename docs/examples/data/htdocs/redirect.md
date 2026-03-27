# redirect.fun (CGI)

- Location: examples/data/htdocs/redirect.fun
- Category: CGI script used by HTTP server examples

Description
- Demonstrates issuing HTTP redirects from a CGI script (setting Status and Location headers).

How to run
- Through one of the HTTP server examples, e.g.:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Open: http://127.0.0.1:8080/redirect.fun

See also
- docs/examples/httpserver.md (deep-dive)
