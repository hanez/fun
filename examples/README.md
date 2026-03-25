# Fun Examples

This directory contains small, pure Fun examples organized by topic.

How to run:
- Build the interpreter once: `cmake --build ./build_debug --target fun`
- Execute an example: `./build_debug/fun ./examples/<path>.fun`
- For CGI examples under `examples/data/htdocs`, start one of the blocking HTTP servers from `examples/blocking/` and visit the URL noted in the file headers.

Categories:
- basics: hello_world, fizzbuzz, fibonacci, collections
- strings: split_join_trim, templating_min, urlencode_decode
- io: read_write_file, csv_reader (uses examples/io/sample.csv), word_count
- algos: sort_and_search, deduplicate, stack_queue
- cli: env_echo, args_parse (parse ARGS env)
- net: tcp_echo_server, tcp_echo_client, http_static_server (blocking)
- data/htdocs (CGI): hello.fun, info.fun, counter.fun, form_post.fun, redirect.fun, json_like_api.fun
- compose: run_other_fun, pipeline
- patterns: assert_like, logging_min
- snippets: escape_html_demo, maps_iterate

Notes:
- Scripts that start servers (tcp_echo_server, http_static_server, http_server_cgi*.fun) are blocking; run them in a separate terminal.
- Some examples rely on stdlib includes; if needed, set `FUN_LIB_DIR=./lib` before running.
