/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
/**
 * @file download.c
 * @brief Fun VM opcode snippet: HTTP download to file via libcurl (OP_CURL_DOWNLOAD).
 *
 * This snippet is included by vm.c and implements the OP_CURL_DOWNLOAD
 * instruction. When FUN_WITH_CURL is enabled, it downloads the content at
 * the given URL and writes it to the specified filesystem path.
 *
 * Stack behavior:
 * - Pops: path:string, url:string (values are converted via value_to_string_alloc)
 * - Pushes: int (1 on success, 0 on error or when CURL is disabled)
 *
 * Pops a destination path and URL, streams the HTTP response body
 * into the file, and pushes 1 on success or 0 on any error. Without
 * FUN_WITH_CURL, behaves as a no-op that consumes two values and
 * pushes 0.
 * 
 * Error handling:
 * - Returns 0 if URL/path conversion fails, file open fails, CURL init
 *   or perform fails.
 * - Follows redirects (CURLOPT_FOLLOWLOCATION = 1L).
 * - Writes via fun_curl_file_write_cb directly into the opened FILE*.
 *
 * Notes:
 * - All temporary allocations (URL, path) are freed; FILE* is closed.
 * - On builds without FUN_WITH_CURL, consumes two values and pushes 0.
 */

case OP_CURL_DOWNLOAD: {
#ifdef FUN_WITH_CURL
  Value vpath = pop_value(vm);
  Value vurl = pop_value(vm);
  char *url = value_to_string_alloc(&vurl);
  char *path = value_to_string_alloc(&vpath);
  free_value(vurl);
  free_value(vpath);
  if (!url || !path) {
    if (url) free(url);
    if (path) free(path);
    push_value(vm, make_int(0));
    break;
  }
  FILE *fp = fopen(path, "wb");
  if (!fp) {
    free(url);
    free(path);
    push_value(vm, make_int(0));
    break;
  }
  CURL *h = curl_easy_init();
  if (!h) {
    fclose(fp);
    free(url);
    free(path);
    push_value(vm, make_int(0));
    break;
  }
  curl_easy_setopt(h, CURLOPT_URL, url);
  curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, fun_curl_file_write_cb);
  curl_easy_setopt(h, CURLOPT_WRITEDATA, fp);
  CURLcode rc = curl_easy_perform(h);
  curl_easy_cleanup(h);
  fclose(fp);
  free(url);
  free(path);
  if (rc != CURLE_OK) {
    push_value(vm, make_int(0));
    break;
  }
  push_value(vm, make_int(1));
#else
  Value a = pop_value(vm);
  free_value(a);
  Value b = pop_value(vm);
  free_value(b);
  push_value(vm, make_int(0));
#endif
  break;
}
