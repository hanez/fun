/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file load.c
 * @brief VM opcode snippet for loading an INI file (OP_INI_LOAD).
 *
 * Opcode: OP_INI_LOAD
 * Stack: [path:string] -> [handle:int]
 *
 * Behavior
 * - Pops a filesystem path and attempts to parse it via iniparser_load().
 * - On success, registers the resulting dictionary and pushes a positive
 *   handle. On failure, pushes 0.
 *
 * Notes
 * - The returned handle must later be released with OP_INI_FREE to avoid
 *   leaking dictionary objects.
 */
/* OP_INI_LOAD: pops path string; pushes handle (>0) or 0 */
#ifdef FUN_WITH_INI
case OP_INI_LOAD: {
  Value vpath = pop_value(vm);
  const char *path = (vpath.type == VAL_STRING && vpath.s) ? vpath.s : NULL;
  int h = 0;
  if (path) {
    dictionary *d = iniparser_load(path);
    if (d) {
      h = ini_alloc_handle(d);
      if (!h) {
        iniparser_freedict(d);
      }
    }
  }
  free_value(vpath);
  push_value(vm, make_int(h));
  break;
}
#endif
