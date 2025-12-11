/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-11 (2025-12-11 migrated from src/vm/libsql/common.c)
 */

/* Ensure libcurl headers and helpers are defined at file scope (not inside vm_run) */
#ifdef FUN_WITH_CURL
#include <curl/curl.h>
typedef struct { char *d; size_t n; } FunCurlBuf;
static size_t fun_curl_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  size_t add = sz * nm;
  FunCurlBuf *b = (FunCurlBuf*)ud;
  char *p = (char*)realloc(b->d, b->n + add + 1);
  if (!p) return 0;
  memcpy(p + b->n, ptr, add);
  b->d = p; b->n += add; b->d[b->n] = '\0';
  return add;
}
static size_t fun_curl_file_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  FILE *f = (FILE*)ud;
  return fwrite(ptr, sz, nm, f);
}
#endif
