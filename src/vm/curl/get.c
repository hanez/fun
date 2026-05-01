/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
/**
 * @file get.c
 * @brief Fun VM opcode snippet: HTTP GET via libcurl (OP_CURL_GET).
 *
 * This snippet is included by vm.c and implements the OP_CURL_GET
 * instruction. When FUN_WITH_CURL is enabled, it performs an HTTP GET
 * request for the provided URL and pushes the response body as a string.
 * If libcurl support is not built in, or an error occurs, an empty string
 * is pushed instead.
 *
 * Stack behavior:
 * - Pops: url:string (any value is converted via value_to_string_alloc)
 * - Pushes: body:string ("" on error or when CURL is disabled)
 *
 * Converts the top stack value to a URL string, issues a GET request
 * using libcurl, and pushes the response body as a string. When
 * compiled without FUN_WITH_CURL, the opcode becomes a no-op that
 * consumes one value and pushes an empty string.
 * 
 * Error handling:
 * - If URL conversion fails or CURL initialization/performance fails,
 *   the opcode pushes an empty string.
 * - Follows HTTP redirects (CURLOPT_FOLLOWLOCATION = 1L).
 *
 * Notes:
 * - Uses FunCurlBuf and fun_curl_write_cb from the curl extension helpers.
 * - Memory allocated for temporary strings and buffers is freed before exit.
 */

case OP_CURL_GET: {
#ifdef FUN_WITH_CURL
  Value vurl = pop_value(vm);
  char *url = value_to_string_alloc(&vurl);
  free_value(vurl);
  if (!url) {
    push_value(vm, make_string(""));
    break;
  }
  FunCurlBuf buf = {NULL, 0};
  CURL *h = curl_easy_init();
  if (!h) {
    free(url);
    push_value(vm, make_string(""));
    break;
  }
  curl_easy_setopt(h, CURLOPT_URL, url);
  curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_write_cb);
  curl_easy_setopt(h, CURLOPT_WRITEDATA, &buf);
  CURLcode rc = curl_easy_perform(h);
  curl_easy_cleanup(h);
  free(url);
  if (rc != CURLE_OK) {
    if (buf.d) free(buf.d);
    push_value(vm, make_string(""));
    break;
  }
  Value s = make_string(buf.d ? buf.d : "");
  if (buf.d) free(buf.d);
  push_value(vm, s);
#else
  Value v = pop_value(vm);
  free_value(v);
  push_value(vm, make_string(""));
#endif
  break;
}
