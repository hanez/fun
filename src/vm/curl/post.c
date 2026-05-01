/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
/**
 * @file post.c
 * @brief Fun VM opcode snippet: HTTP POST via libcurl (OP_CURL_POST).
 *
 * This snippet is included by vm.c and implements the OP_CURL_POST
 * instruction. When FUN_WITH_CURL is enabled, it performs an HTTP POST
 * to the given URL with the provided request body and pushes the response
 * body as a string. If libcurl support is not built in, or an error occurs,
 * an empty string is pushed instead.
 *
 * Stack behavior:
 * - Pops: body:string, url:string (values are converted via value_to_string_alloc)
 * - Pushes: body:string (server response; "" on error or when CURL is disabled)
 *
 * Pops the POST body and URL, performs an HTTP POST, and pushes the
 * response as a string. Without FUN_WITH_CURL, consumes two values and
 * pushes an empty string.
 * 
 * Error handling:
 * - If URL conversion fails, the opcode pushes an empty string and discards
 *   any converted body.
 * - Follows redirects (CURLOPT_FOLLOWLOCATION = 1L).
 * - Sets CURLOPT_POST=1L and CURLOPT_POSTFIELDS to submit the body.
 *
 * Notes:
 * - Uses FunCurlBuf and fun_curl_write_cb from the curl extension helpers.
 * - All temporary allocations (URL, body, response buffer) are freed.
 */

case OP_CURL_POST: {
#ifdef FUN_WITH_CURL
  Value vbody = pop_value(vm);
  Value vurl = pop_value(vm);
  char *url = value_to_string_alloc(&vurl);
  char *body = value_to_string_alloc(&vbody);
  free_value(vurl);
  free_value(vbody);
  if (!url) {
    if (body) free(body);
    push_value(vm, make_string(""));
    break;
  }
  if (!body) body = strdup("");
  FunCurlBuf buf = {NULL, 0};
  CURL *h = curl_easy_init();
  if (!h) {
    free(url);
    free(body);
    push_value(vm, make_string(""));
    break;
  }
  curl_easy_setopt(h, CURLOPT_URL, url);
  curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(h, CURLOPT_POST, 1L);
  curl_easy_setopt(h, CURLOPT_POSTFIELDS, body);
  curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_write_cb);
  curl_easy_setopt(h, CURLOPT_WRITEDATA, &buf);
  CURLcode rc = curl_easy_perform(h);
  curl_easy_cleanup(h);
  free(url);
  free(body);
  if (rc != CURLE_OK) {
    if (buf.d) free(buf.d);
    push_value(vm, make_string(""));
    break;
  }
  Value s = make_string(buf.d ? buf.d : "");
  if (buf.d) free(buf.d);
  push_value(vm, s);
#else
  Value a = pop_value(vm);
  free_value(a);
  Value b = pop_value(vm);
  free_value(b);
  push_value(vm, make_string(""));
#endif
  break;
}
