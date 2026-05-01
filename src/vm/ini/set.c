/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file set.c
 * @brief VM opcode snippet for setting an INI value (OP_INI_SET).
 *
 * Opcode: OP_INI_SET
 * Stack: [value:any] [key:string] [section:string] [handle:int] -> [ok:int]
 *
 * Behavior
 * - Pops value, key, section, and handle. Converts the value to a string using
 *   value_to_string_alloc() and stores it under "section:key" (and a dotted
 *   fallback) via dictionary_set().
 * - Pushes 1 on success, 0 on failure (invalid args/handle or allocation fail).
 *
 * Errors
 * - No VM exception is thrown; failures return 0.
 */
/* OP_INI_SET */
#ifdef FUN_WITH_INI
case OP_INI_SET: {
  Value vval = pop_value(vm);
  Value vkey = pop_value(vm);
  Value vsec = pop_value(vm);
  Value vh = pop_value(vm);
  dictionary *d = ini_get((vh.type == VAL_INT) ? (int)vh.i : 0);
  const char *key = (vkey.type == VAL_STRING) ? vkey.s : NULL;
  const char *sec = (vsec.type == VAL_STRING) ? vsec.s : NULL;
  int ok = 0;
  if (d && sec && key) {
    char *valstr = value_to_string_alloc(&vval);
    if (valstr) {
      char full[1024];
      char alt[1024];
      ini_make_full_key(full, sizeof(full), sec, key);
      memcpy(alt, full, sizeof(alt));
      for (size_t i = 0; i < sizeof(alt) && alt[i]; ++i) {
        if (alt[i] == ':') {
          alt[i] = '.';
          break;
        }
      }
      /* iniparser 4.x does not expose iniparser_set; use dictionary_set */
      if (dictionary_set(d, full, valstr) == 0) {
        ok = 1; /* 0 means success */
      } else if (dictionary_set(d, alt, valstr) == 0) {
        ok = 1;
      }
      free(valstr);
    }
  }
  free_value(vval);
  free_value(vkey);
  free_value(vsec);
  free_value(vh);
  push_value(vm, make_int(ok));
  break;
}
#endif
