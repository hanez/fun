# form_post.fun (CGI)

- Location: examples/data/htdocs/form_post.fun
- Category: CGI script used by HTTP server examples

Description
- Demonstrates handling of POST form data via the CGI interface.

How to run
- Through one of the HTTP server examples, e.g.:
  - export FUN_LIB_DIR="./lib"
  - export FUN_EXEC="./build_debug/fun"
  - ./build_debug/fun examples/net/http_server_cgi.fun
  - Submit a form to: http://127.0.0.1:8080/form_post.fun

See also
- docs/examples/httpserver.md (deep-dive)
