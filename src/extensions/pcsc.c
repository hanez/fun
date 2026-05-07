/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file pcsc.c
 * @brief PC/SC smartcard helper registries and lookup utilities for VM opcodes.
 *
 * This module centralizes tiny fixed-size registries for PC/SC resources and
 * minimal helper functions used by VM opcodes under src/vm/pcsc/*.c. Keeping
 * the concrete handle management here allows the opcode implementations to
 * focus on VM stack marshalling, mirroring the approach taken by other
 * extensions (PCRE2, SQLite, XML2, JSON, INI, cURL, OpenSSL).
 *
 * Build-time feature flag:
 * - All code in this file is compiled only when FUN_WITH_PCSC is enabled. When
 *   disabled, the referencing opcodes should compile to safe no-op fallbacks
 *   (typically pushing 0, Nil, or empty arrays) while retaining the same stack
 *   behaviour.
 *
 * Registries and ownership model:
 * - Context registry (g_pcsc_ctx): stores SCARDCONTEXT values obtained via
 *   SCardEstablishContext(). The registry DOES NOT call SCardReleaseContext()
 *   automatically — releasing is the responsibility of the opcode that owns
 *   the lifecycle. The registry merely tracks usage and provides a stable,
 *   small integer id for referencing a context in later operations.
 * - Card registry (g_pcsc_card): stores SCARDHANDLE and the negotiated
 *   protocol (proto) obtained via SCardConnect(). Similarly, the registry does
 *   not call SCardDisconnect(); the VM opcode that created the handle is in
 *   charge of eventual teardown.
 * - Handles are 1-based indices into the fixed arrays (contexts: up to 8,
 *   cards: up to 32). A return value of 0 indicates failure (no free slot or
 *   invalid request).
 *
 * Error handling and limits:
 * - Allocation helpers return 0 when no free slot is available. Lookup helpers
 *   return NULL if the id is out of range or the slot is not currently in use.
 * - The fixed sizes (8 contexts, 32 cards) are pragmatic defaults for typical
 *   scripts. Increase cautiously if your workloads require more concurrent
 *   resources.
 *
 * Thread-safety:
 * - This module is not thread-safe. If the interpreter is used from multiple
 *   threads, coordinate access to these registries externally.
 */

#ifdef FUN_WITH_PCSC
#if defined(__has_include)
#if __has_include(<PCSC/winscard.h>)
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#elif __has_include(<winscard.h>)
#include <winscard.h>
#else
#error "FUN_WITH_PCSC is enabled but PCSC headers were not found"
#endif
#else
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#endif
#include <string.h>
#include <stdio.h>

/**
 * @brief One slot in the PC/SC context registry.
 *
 * Ownership notes:
 * - The registry does not own the context; it merely stores the value returned
 *   by SCardEstablishContext(). Callers/opcodes are responsible for invoking
 *   SCardReleaseContext() at the appropriate time.
 */
typedef struct {
  SCARDCONTEXT ctx; /**< Established context value. */
  int in_use;       /**< 1 if the slot is currently allocated; 0 otherwise. */
} pcsc_ctx_entry;

/**
 * @brief One slot in the PC/SC card handle registry.
 *
 * Ownership notes:
 * - The registry does not disconnect cards; it only stores the handle returned
 *   by SCardConnect() and the negotiated protocol. The VM opcode that created
 *   the handle is responsible for calling SCardDisconnect().
 */
typedef struct {
  SCARDHANDLE h; /**< Connected card handle from SCardConnect(). */
  DWORD proto;   /**< Negotiated protocol flags (e.g., SCARD_PROTOCOL_T0/T1). */
  int in_use;    /**< 1 if the slot is currently allocated; 0 otherwise. */
} pcsc_card_entry;

static pcsc_ctx_entry g_pcsc_ctx[8];
static pcsc_card_entry g_pcsc_card[32];

/**
 * @brief Allocate a free context slot in the PC/SC registry.
 *
 * Scans the small fixed-size context registry for an available slot, marks it
 * as in use, clears the stored value, and returns a 1-based identifier.
 *
 * @return int A 1-based slot id on success; 0 if no free slot is available.
 */
static int pcsc_alloc_ctx_slot(void) {
  for (int i = 0; i < (int)(sizeof(g_pcsc_ctx) / sizeof(g_pcsc_ctx[0])); ++i) {
    if (!g_pcsc_ctx[i].in_use) {
      g_pcsc_ctx[i].in_use = 1;
      g_pcsc_ctx[i].ctx = 0;
      return i + 1;
    }
  }
  return 0;
}

/**
 * @brief Allocate a free card slot in the PC/SC registry.
 *
 * Scans the card registry for an unused entry, sets initial values, and
 * returns a small positive identifier that can be used by opcodes to index the
 * slot later.
 *
 * @return int A 1-based slot id on success; 0 if no free slot is available.
 */
static int pcsc_alloc_card_slot(void) {
  for (int i = 0; i < (int)(sizeof(g_pcsc_card) / sizeof(g_pcsc_card[0])); ++i) {
    if (!g_pcsc_card[i].in_use) {
      g_pcsc_card[i].in_use = 1;
      g_pcsc_card[i].h = 0;
      g_pcsc_card[i].proto = 0;
      return i + 1;
    }
  }
  return 0;
}

/**
 * @brief Lookup a context slot by id.
 *
 * Validates the provided 1-based identifier, ensures the slot is currently in
 * use, and returns a pointer to the internal registry entry.
 *
 * @param id int 1-based context id previously returned by pcsc_alloc_ctx_slot().
 * @return pcsc_ctx_entry* Pointer to the entry if valid and in use; NULL otherwise.
 */
static pcsc_ctx_entry *pcsc_get_ctx(int id) {
  if (id <= 0) return 0;
  int idx = id - 1;
  if (idx < 0 || idx >= (int)(sizeof(g_pcsc_ctx) / sizeof(g_pcsc_ctx[0]))) return 0;
  if (!g_pcsc_ctx[idx].in_use) return 0;
  return &g_pcsc_ctx[idx];
}

/**
 * @brief Lookup a card slot by id.
 *
 * Validates the provided 1-based identifier, ensures the slot is currently in
 * use, and returns a pointer to the internal registry entry, which exposes the
 * SCARDHANDLE and negotiated protocol for subsequent operations (e.g.,
 * SCardTransmit()).
 *
 * @param id int 1-based card id previously returned by pcsc_alloc_card_slot().
 * @return pcsc_card_entry* Pointer to the entry if valid and in use; NULL otherwise.
 */
static pcsc_card_entry *pcsc_get_card(int id) {
  if (id <= 0) return 0;
  int idx = id - 1;
  if (idx < 0 || idx >= (int)(sizeof(g_pcsc_card) / sizeof(g_pcsc_card[0]))) return 0;
  if (!g_pcsc_card[idx].in_use) return 0;
  return &g_pcsc_card[idx];
}
#endif
