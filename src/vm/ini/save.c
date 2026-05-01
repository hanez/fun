/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file save.c
 * @brief VM opcode snippet for saving an INI dictionary to a file (OP_INI_SAVE).
 *
 * Opcode: OP_INI_SAVE
 * Stack: [path:string] [handle:int] -> [ok:int]
 *
 * Behavior
 * - Pops a path and a handle. If the handle is valid, opens the path for
 *   writing and dumps the dictionary in INI format. Pushes 1 on success,
 *   otherwise 0.
 *
 * Errors
 * - Failing fopen() or invalid handle simply return 0; no exception is thrown.
 */

/* OP_INI_SAVE */
#ifdef FUN_WITH_INI
case OP_INI_SAVE: {
  Value vpath = pop_value(vm);
  Value vh = pop_value(vm);
  const char *path = (vpath.type == VAL_STRING) ? vpath.s : NULL;
  dictionary *d = ini_get((vh.type == VAL_INT) ? (int)vh.i : 0);
  int ok = 0;
  if (d && path) {
    FILE *f = fopen(path, "w");
    if (f) {
      iniparser_dump_ini(d, f);
      fclose(f);
      ok = 1;
    }
  }
  free_value(vpath);
  free_value(vh);
  push_value(vm, make_int(ok));
  break;
}
#endif
