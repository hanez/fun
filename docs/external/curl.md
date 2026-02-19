# cURL (libcurl) extension (optional)

- CMake option: FUN_WITH_CURL=ON
- Purpose: HTTP helpers using libcurl.
- Homepage: https://curl.se/libcurl/

Opcodes:
- OP_CURL_GET: GET; pops url:string; pushes body:string (empty on error/disabled)
- OP_CURL_POST: POST; pops body:string, url:string; pushes response:string
- OP_CURL_DOWNLOAD: Download to file; pops path:string, url:string; pushes 1/0

Notes:
- Requires libcurl development headers/libs.
- When disabled, helpers return empty strings/0 to match optional behavior.
