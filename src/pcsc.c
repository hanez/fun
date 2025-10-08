/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */
 
 /* PCSC helpers: registries and helper functions.
 * Included at file scope from vm.c.
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

  typedef struct {
      SCARDCONTEXT ctx;
      int in_use;
  } pcsc_ctx_entry;

  typedef struct {
      SCARDHANDLE h;
      DWORD proto;
      int in_use;
  } pcsc_card_entry;

  static pcsc_ctx_entry g_pcsc_ctx[8];
  static pcsc_card_entry g_pcsc_card[32];

  static int pcsc_alloc_ctx_slot(void) {
      for (int i = 0; i < (int)(sizeof(g_pcsc_ctx)/sizeof(g_pcsc_ctx[0])); ++i) {
          if (!g_pcsc_ctx[i].in_use) { g_pcsc_ctx[i].in_use = 1; g_pcsc_ctx[i].ctx = 0; return i + 1; }
      }
      return 0;
  }

  static int pcsc_alloc_card_slot(void) {
      for (int i = 0; i < (int)(sizeof(g_pcsc_card)/sizeof(g_pcsc_card[0])); ++i) {
          if (!g_pcsc_card[i].in_use) { g_pcsc_card[i].in_use = 1; g_pcsc_card[i].h = 0; g_pcsc_card[i].proto = 0; return i + 1; }
      }
      return 0;
  }

  static pcsc_ctx_entry* pcsc_get_ctx(int id) {
      if (id <= 0) return NULL;
      int idx = id - 1;
      if (idx < 0 || idx >= (int)(sizeof(g_pcsc_ctx)/sizeof(g_pcsc_ctx[0]))) return NULL;
      if (!g_pcsc_ctx[idx].in_use) return NULL;
      return &g_pcsc_ctx[idx];
  }

  static pcsc_card_entry* pcsc_get_card(int id) {
      if (id <= 0) return NULL;
      int idx = id - 1;
      if (idx < 0 || idx >= (int)(sizeof(g_pcsc_card)/sizeof(g_pcsc_card[0]))) return NULL;
      if (!g_pcsc_card[idx].in_use) return NULL;
      return &g_pcsc_card[idx];
  }
#endif /* FUN_WITH_PCSC */
