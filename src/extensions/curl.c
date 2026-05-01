/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file curl.c
 * @brief libcurl helpers and buffers used by HTTP-related VM opcodes.
 *
 * Declares small buffer helpers and libcurl write callbacks that support
 * network-related opcodes when FUN_WITH_CURL is enabled.
 */

/* Ensure libcurl headers and helpers are defined at file scope (not inside vm_run) */
#ifdef FUN_WITH_CURL
#include <curl/curl.h>

/**
 * @brief Simple growable buffer for libcurl write callbacks.
 *
 * The buffer grows via realloc as more data arrives. The content is kept
 * NUL-terminated for convenience.
 */
typedef struct {
  char *d;   /**< Data pointer (NUL-terminated). */
  size_t n;  /**< Number of bytes stored (excluding NUL). */
} FunCurlBuf;

/**
 * @brief libcurl write callback that appends data to a FunCurlBuf.
 *
 * Reallocates the destination buffer as needed and keeps it NUL-terminated.
 *
 * @param ptr Pointer to incoming data block provided by libcurl.
 * @param sz Size of each data element.
 * @param nm Number of elements in this block.
 * @param ud User data; must be a FunCurlBuf*.
 * @return Number of bytes actually handled (sz*nm) on success, 0 on failure
 *         to signal an error to libcurl.
 */
static size_t fun_curl_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  size_t add = sz * nm;
  FunCurlBuf *b = (FunCurlBuf *)ud;
  char *p = (char *)realloc(b->d, b->n + add + 1);
  if (!p) return 0;
  memcpy(p + b->n, ptr, add);
  b->d = p;
  b->n += add;
  b->d[b->n] = '\0';
  return add;
}

/**
 * @brief libcurl write callback that writes directly to a FILE*.
 *
 * @param ptr Pointer to incoming data.
 * @param sz Size of each element.
 * @param nm Number of elements.
 * @param ud User data; must be a FILE* opened for writing in binary mode.
 * @return Number of elements written (as returned by fwrite).
 */
static size_t fun_curl_file_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  FILE *f = (FILE *)ud;
  return fwrite(ptr, sz, nm, f);
}
#endif
