/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file get_string.c
 * @brief VM opcode snippet for reading a string from INI (OP_INI_GET_STRING).
 *
 * Opcode: OP_INI_GET_STRING
 * Stack: [default:string] [key:string] [section:string] [handle:int] -> [out:string]
 *
 * Behavior
 * - Pops default string, key, section, and handle; looks up "section:key" and
 *   a dotted fallback. If not found, uses the provided default.
 * - Pushes the resulting string (copied into a VM Value).
 *
 * Errors
 * - Invalid handle/args result in pushing the default (or empty string).
 */
/* OP_INI_GET_STRING */
#ifdef FUN_WITH_INI
case OP_INI_GET_STRING: {
  Value vdef = pop_value(vm);
  Value vkey = pop_value(vm);
  Value vsec = pop_value(vm);
  Value vh = pop_value(vm);
  const char *def = (vdef.type == VAL_STRING && vdef.s) ? vdef.s : "";
  const char *key = (vkey.type == VAL_STRING) ? vkey.s : NULL;
  const char *sec = (vsec.type == VAL_STRING) ? vsec.s : NULL;
  int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
  dictionary *d = ini_get(h);
  const char *res = def;
  if (d && sec && key) {
    char full[1024];
    char alt[1024];
    ini_make_full_key(full, sizeof(full), sec, key);
    /* Build alternate with dot separator for robustness */
    ini_make_full_key(alt, sizeof(alt), sec, key);
    size_t flen = strlen(full);
    if (flen < sizeof(alt) && flen > 0) { /* create dot version in alt */
      memcpy(alt, full, flen + 1);
      for (size_t i = 0; i < flen; ++i)
        if (alt[i] == ':') {
          alt[i] = '.';
          break;
        }
    }
    const char *s = iniparser_getstring(d, full, def);
    if (s == def) { /* not found, try alternate dot form */
      s = iniparser_getstring(d, alt, def);
    }
    res = s ? s : "";
  }
  free_value(vdef);
  free_value(vkey);
  free_value(vsec);
  free_value(vh);
  push_value(vm, make_string(res));
  break;
}
#endif
