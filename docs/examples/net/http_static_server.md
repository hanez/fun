# http_static_server.fun

- Location: examples/net/http_static_server.fun
- Category: Networking / Sockets

Description
- Minimal static HTTP server implemented directly on sockets. Accepts connections and always returns a small HTML page.

How to run
- From the repository root:
  - export FUN_LIB_DIR="./lib"
  - ./build_debug/fun examples/net/http_static_server.fun
  - Then open http://127.0.0.1:8088/

Requirements
- Uses core IO/socket stdlib (io/socket.fun). No optional extensions required.

See also
- docs/examples/README.md
- docs/examples/httpserver.md (deep-dive over HTTP servers)
