/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file redis.c
 * @brief Hiredis handle registry and reply mapping helpers for the Fun VM.
 *
 * This translation unit provides two small building blocks used by the Redis
 * opcodes (implemented under src/vm/redis/*.c) and included from src/vm.c:
 *
 * 1) A process-local registry that assigns monotonically increasing positive
 *    integer identifiers to hiredis connection pointers (redisContext*).
 *    VM opcodes pass integer ids on the stack instead of raw pointers, keeping
 *    the bytecode portable and preventing accidental misuse of pointers.
 *
 * 2) Utilities to convert hiredis reply objects (redisReply) into Fun VM
 *    Value instances, recursively mapping arrays and supporting basic numeric
 *    and string types.
 *
 * Build-time feature flag
 * -----------------------
 * The code is compiled only when the CMake option FUN_WITH_REDIS is enabled
 * (i.e., the preprocessor symbol FUN_WITH_REDIS is defined). When disabled,
 * this file contributes no symbols and the corresponding opcodes are compiled
 * into stubs that return neutral values.
 *
 * Ownership and lifetime
 * ----------------------
 * - The registry does NOT open or close Redis connections by itself; it merely
 *   stores pointers created elsewhere (e.g., via redisConnectWithTimeout()).
 * - Adding an entry does not transfer ownership of the redisContext. Callers
 *   remain responsible for invoking redisFree() at the appropriate time.
 * - Removing an entry from the registry does NOT free the connection; it only
 *   forgets the mapping between id and pointer. The connect/close opcodes take
 *   care of proper ownership transitions.
 * - Integer identifiers are monotonically increasing per process. Once a
 *   handle id is deleted, it will not be reused within the same process
 *   lifetime.
 *
 * Error handling
 * --------------
 * Functions here perform basic validation/allocations only. Allocation
 * failures return NULL (for lookups/additions) or are silently ignored (for
 * deletions of non-existent ids). No hiredis API calls are made here, so no
 * hiredis error codes are produced by this module itself.
 *
 * Thread-safety
 * -------------
 * The registry is a simple singly-linked list with no synchronization. It is
 * NOT thread-safe. If the VM uses Redis from multiple threads, the caller must
 * provide external synchronization around calls to these helpers.
 *
 * Example
 * -------
 * @code{.c}
 * // Open a hiredis connection elsewhere:
 * struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
 * redisContext *ctx = redisConnectWithTimeout("127.0.0.1", 6379, tv);
 * if (ctx && !ctx->err) {
 *   // Register and get an id:
 *   RedisHandle *h = redis_reg_add(ctx);
 *   int id = h ? h->id : -1;
 *
 *   // Later look it up:
 *   RedisHandle *same = redis_reg_get(id);
 *   if (same) {
 *     // use same->ctx with hiredis APIs
 *   }
 *
 *   // When finished, drop the registry entry and free manually:
 *   redis_reg_del(id);
 *   redisFree(ctx);
 * }
 * @endcode
 */

#ifdef FUN_WITH_REDIS
#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations from the VM (available in the same TU via includes) */
static Value hiredis_reply_to_value(const redisReply *r);

/**
 * @brief Node in a singly-linked list of registered Redis handles.
 *
 * Associates a monotonically increasing positive integer identifier with a raw
 * hiredis connection pointer. The list head is stored in a file-static global
 * (g_redis_handles).
 */
typedef struct RedisHandle {
  int id;                  /**< Positive identifier assigned by the registry. */
  redisContext *ctx;       /**< Opaque pointer to a hiredis connection. */
  struct RedisHandle *next;/**< Next entry in the singly-linked list. */
} RedisHandle;

/** @brief Global head of the Redis handle list (NULL denotes empty list). */
static RedisHandle *g_redis_handles = NULL;
/** @brief Next positive identifier to assign to a newly added handle. */
static int g_redis_next_id = 1;

/**
 * @brief Add a hiredis connection handle to the registry.
 *
 * Allocates a new list node, assigns a fresh positive id, and prepends it to
 * the internal registry list. Ownership of the redisContext remains with the
 * caller; this registry does not free it during deletion.
 *
 * @param ctx Valid pointer to an opened hiredis connection.
 * @return Pointer to the newly created RedisHandle on success; NULL on
 *         allocation failure. The returned pointer remains owned by the
 *         registry; do not free it directly.
 */
static RedisHandle *redis_reg_add(redisContext *ctx) {
  RedisHandle *h = (RedisHandle *)calloc(1, sizeof(RedisHandle));
  if (!h) return NULL;
  h->id = g_redis_next_id++;
  h->ctx = ctx;
  h->next = g_redis_handles;
  g_redis_handles = h;
  return h;
}

/**
 * @brief Look up a registered Redis handle by id.
 *
 * Performs a linear search over the internal list to find a matching id.
 *
 * @param id Positive identifier previously returned by redis_reg_add().
 * @return Pointer to the RedisHandle entry if found; NULL otherwise.
 *
 * @note The returned pointer is owned by the registry and must not be freed by
 *       the caller.
 */
static RedisHandle *redis_reg_get(int id) {
  for (RedisHandle *p = g_redis_handles; p; p = p->next)
    if (p->id == id) return p;
  return NULL;
}

/**
 * @brief Remove a Redis handle entry from the registry.
 *
 * Deletes the list node associated with the given id.
 *
 * @param id Positive identifier of the entry to remove.
 *
 * @note This function does not free the underlying redisContext; the caller is
 *       responsible for calling redisFree() if appropriate.
 * @note If the id does not exist, the function is a no-op.
 */
static void redis_reg_del(int id) {
  RedisHandle **pp = &g_redis_handles;
  while (*pp) {
    if ((*pp)->id == id) {
      RedisHandle *d = *pp;
      *pp = d->next;
      free(d);
      return;
    }
    pp = &(*pp)->next;
  }
}

/**
 * @brief Convert a hiredis reply to a Fun Value.
 *
 * Recursively maps hiredis reply types to the closest Fun representation:
 * - REDIS_REPLY_STRING / STATUS -> string
 * - REDIS_REPLY_INTEGER         -> int
 * - REDIS_REPLY_NIL             -> nil
 * - REDIS_REPLY_ARRAY           -> array of recursively converted values
 * - REDIS_REPLY_DOUBLE (if available) -> float
 * - REDIS_REPLY_ERROR / default -> string (error text or "ERR")
 *
 * @param r Non-owning pointer to a redisReply.
 * @return Value converted from the reply. For NULL replies, returns nil.
 */
static Value hiredis_reply_to_value(const redisReply *r) {
  if (!r) return make_nil();
  switch (r->type) {
    case REDIS_REPLY_STRING:
    case REDIS_REPLY_STATUS:
      return make_string(r->str ? r->str : "");
    case REDIS_REPLY_INTEGER:
      return make_int((int64_t)r->integer);
    case REDIS_REPLY_NIL:
      return make_nil();
    case REDIS_REPLY_ARRAY: {
      int n = (int)r->elements;
      if (n <= 0) {
        return make_array_from_values(NULL, 0);
      }
      Value *items = (Value *)calloc((size_t)n, sizeof(Value));
      if (!items) return make_array_from_values(NULL, 0);
      for (int i = 0; i < n; i++) {
        items[i] = hiredis_reply_to_value(r->element[i]);
      }
      Value arr = make_array_from_values(items, n);
      for (int i = 0; i < n; i++) free_value(items[i]);
      free(items);
      return arr;
    }
#ifdef REDIS_REPLY_DOUBLE
    case REDIS_REPLY_DOUBLE:
      return make_float(r->dval);
#endif
    case REDIS_REPLY_ERROR:
    default:
      return make_string(r->str ? r->str : "ERR");
  }
}

#endif /* FUN_WITH_REDIS */
