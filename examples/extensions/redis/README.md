Redis extension examples (hiredis)

This folder contains small Fun scripts that demonstrate how to use the Redis extension.

Prerequisites
- Build Fun with Redis support enabled (FUN_WITH_REDIS=ON). This is ON by default in cmake/Extensions/REDIS.cmake.
- A Redis-compatible server reachable at 127.0.0.1:6379.

How to run
- Using the Fun CLI from the repository root:
  - Debug profile path: build_debug/fun
  - Release profile path: build_release/fun

Examples
1. basic_ping.fun
   - Connects, PINGs, then closes.

2. kv_set_get.fun
   - SET/GET, EXISTS and DEL for a demo key.

3. list_ops.fun
   - Demonstrates LPUSH and LRANGE on a list.

4. hash_ops.fun
   - Demonstrates HSET, HGET and HGETALL on a hash.

Note
- All examples use direct inline command strings with redis_cmd(handle, "COMMAND args...").
- Close the connection with redis_close(handle) when finished.
