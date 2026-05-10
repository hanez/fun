/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file kcgi.c
 * @brief Thin kcgi integration helpers used by VM opcodes under src/vm/kcgi/.
 *
 * This module centralizes compact helpers around the kcgi API so the VM
 * opcode snippets included by vm.c can remain minimal and focus on stack
 * marshalling. The helpers here do not depend on VM internals beyond the
 * Value conversion utilities used to represent HTTP requests as Fun values.
 * Keeping the external-API specifics in src/extensions/ mirrors other
 * integrations (OpenSSL, PCRE2, SQLite, XML2, JSON, INI) and improves
 * maintainability.
 *
 * Build-time feature flag:
 * - All code in this file is compiled only when FUN_WITH_KCGI is enabled.
 *   When disabled, the file provides no symbols and VM opcodes compiled
 *   from src/vm/kcgi/*.c will fall back to no-op/falsey behaviors as
 *   documented in those opcode snippets.
 *
 * Ownership and memory model:
 * - kcgi_parse_request() allocates a struct kreq via khttp_parse() and, on
 *   success, returns it through the out-parameter; the caller takes ownership
 *   and must later release it with kcgi_free_request().
 * - kcgi_free_request() calls khttp_free() and then frees the allocation
 *   wrapper used by this module.
 * - kcgi_write_str() does not take ownership of the input string; it may be
 *   NULL, which is treated as an empty string.
 * - kreq_to_fun() and kcgi_fields_to_map() create Fun Values following the
 *   VM's normal ownership semantics (returned by value to the caller).
 *
 * Global state and lifetime:
 * - A thread-local pointer g_kcgi_req holds the current request for the
 *   active CGI handling context. VM opcodes are responsible for setting and
 *   clearing this pointer by calling the helpers exposed here.
 *
 * Error handling:
 * - Functions return 0/NULL on failure and non-zero/valid objects on success.
 *   Callers should check return values before use. No errno is set.
 *
 * Thread-safety:
 * - The request handle is stored in a thread-local variable to isolate state
 *   between concurrent executions. Helpers otherwise maintain no global
 *   mutable state.
 */

#ifdef FUN_WITH_KCGI

#include <kcgi.h>
#include <stdlib.h>
#include <string.h>

/* Thread-local request handle for the current CGI invocation */
#ifdef _WIN32
static __declspec(thread) struct kreq *g_kcgi_req = NULL;
#else
static __thread struct kreq *g_kcgi_req = NULL;
#endif

/**
 * Convert kcgi form/query fields to a Fun map Value.
 *
 * Each entry from r->fields is copied into a newly created Fun map where
 * the kcgi key becomes the map key and the kcgi value becomes a Fun string.
 *
 * Notes:
 * - If the request pointer is NULL, an empty map is returned.
 * - When multiple kcgi fields share the same key, later occurrences will
 *   overwrite earlier ones ("last write wins").
 * - Empty or NULL keys/values are converted to empty strings ("").
 *
 * Ownership:
 * - The returned Value is owned by the caller (normal VM semantics).
 *
 * @param r  Parsed kcgi request pointer (may be NULL).
 * @return   A Fun map Value containing string keys and values.
 */
static Value kcgi_fields_to_map(const struct kreq *r) {
  Value m = make_map_empty();
  if (!r) return m;
  for (size_t i = 0; i < r->fieldsz; i++) {
    const char *k = r->fields[i].key ? r->fields[i].key : "";
    const char *v = r->fields[i].val ? r->fields[i].val : "";
    map_set(&m, k, make_string(v));
  }
  return m;
}

/**
 * Convert a kcgi request handle into a structured Fun Value.
 *
 * The resulting map contains at least the following keys:
 * - "host":   string; empty if unavailable
 * - "port":   int;    TCP port
 * - "path":   string; request path
 * - "suffix": string; request suffix (kcgi notion)
 * - "fields": map;    key/value pairs from form/query fields
 *
 * Missing fields from kcgi are converted to empty strings where applicable.
 * If r is NULL, an empty map is returned.
 *
 * @param r  Parsed kcgi request pointer (may be NULL).
 * @return   A Fun map Value describing the request.
 */
static Value kreq_to_fun(const struct kreq *r) {
  Value out = make_map_empty();
  if (!r) return out;
  /* Basic request info */
  map_set(&out, "host",   make_string(r->host ? r->host : ""));
  map_set(&out, "port",   make_int((int64_t)r->port));
  map_set(&out, "path",   make_string(r->path ? r->path : ""));
  map_set(&out, "suffix", make_string(r->suffix ? r->suffix : ""));

  Value fields = kcgi_fields_to_map(r);
  map_set(&out, "fields", fields);
  return out;
}

/* Lifecycle helpers used by VM opcodes */
/**
 * Parse the current CGI/FCGI request using kcgi.
 *
 * On success, this allocates and initializes a struct kreq by calling
 * khttp_parse() and stores it in *out. The caller takes ownership of the
 * returned handle and must later free it with kcgi_free_request().
 *
 * Behavior details:
 * - All keys are accepted (kvalid array with a single {NULL,NULL} entry).
 * - On allocation or parse failure, *out is not modified and 0 is returned.
 *
 * Threading:
 * - This function is independent of the thread-local g_kcgi_req; setting that
 *   pointer is left to the caller/opcode.
 *
 * @param[out] out  Where to store the newly allocated struct kreq on success.
 * @return          1 on success, 0 on failure.
 */
static int kcgi_parse_request(struct kreq **out) {
  static const struct kvalid keys[] = { { NULL, NULL } }; /* accept all */
  struct kreq *r = (struct kreq *)calloc(1, sizeof(*r));
  if (!r) return 0;
  enum kcgi_err er = khttp_parse(r, keys, 0, NULL, 0, 0);
  if (er != KCGI_OK) { free(r); return 0; }
  *out = r;
  return 1;
}

/**
 * Free a request previously returned by kcgi_parse_request().
 *
 * Safe to call with NULL.
 *
 * @param r  Request handle to free (may be NULL).
 */
static void kcgi_free_request(struct kreq *r) {
  if (!r) return;
  khttp_free(r);
  free(r);
}

/**
 * Begin the HTTP response for the current request.
 *
 * This emits an optional Content-Type header and then switches into body
 * mode by calling khttp_body(). If g_kcgi_req is not set, the call fails.
 *
 * Notes:
 * - The status code parameter is currently not passed to kcgi; unless set
 *   elsewhere, kcgi will default the status to 200 OK.
 * - If ctype is NULL or empty, no Content-Type header is emitted here.
 *
 * @param code   Suggested HTTP status code (reserved for future use).
 * @param ctype  MIME type to emit as Content-Type (may be NULL/empty).
 * @return       1 on success, 0 on failure.
 */
static int kcgi_reply_start(int code, const char *ctype) {
  if (!g_kcgi_req) return 0;
  /* Emit Content-Type header; Status defaults to 200 if not set */
  if (ctype && *ctype) {
    if (khttp_head(g_kcgi_req, "Content-Type", "%s", ctype) != KCGI_OK)
      return 0;
  }
  if (khttp_body(g_kcgi_req) != KCGI_OK)
    return 0;
  return 1;
}

/**
 * Write a UTF-8 string to the HTTP response body.
 *
 * Requires an active request in g_kcgi_req and an initialized response body
 * (kcgi_reply_start() or equivalent must have been called).
 *
 * @param s  NUL-terminated string to write; NULL is treated as "".
 * @return   1 on success, 0 on failure.
 */
static int kcgi_write_str(const char *s) {
  if (!g_kcgi_req) return 0;
  if (!s) s = "";
  size_t n = strlen(s);
  return khttp_write(g_kcgi_req, s, n) == KCGI_OK;
}

#endif /* FUN_WITH_KCGI */
