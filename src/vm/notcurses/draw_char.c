/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* NC_DRAW_CHAR */
case OP_NC_DRAW_CHAR: {
  Value yv = pop_value(vm);
  Value xv = pop_value(vm);
  Value chv = pop_value(vm);
  int y = (yv.type == VAL_INT ? (int)yv.i : (yv.type == VAL_FLOAT ? (int)yv.d : 0));
  int x = (xv.type == VAL_INT ? (int)xv.i : (xv.type == VAL_FLOAT ? (int)xv.d : 0));
  uint32_t cp = (chv.type == VAL_INT ? (uint32_t)chv.i : (chv.type == VAL_FLOAT ? (uint32_t)chv.d : (uint32_t)'?'));
  free_value(yv);
  free_value(xv);
  free_value(chv);
#ifdef FUN_WITH_NOTCURSES
  if (_fun_nc && _fun_nc_std) {
    char utf8[5];
    int len = 0;
    if (cp <= 0x7F) { utf8[0] = (char)cp; len = 1; }
    else if (cp <= 0x7FF) { utf8[0] = (char)(0xC0 | (cp >> 6)); utf8[1] = (char)(0x80 | (cp & 0x3F)); len = 2; }
    else if (cp <= 0xFFFF) { utf8[0] = (char)(0xE0 | (cp >> 12)); utf8[1] = (char)(0x80 | ((cp >> 6) & 0x3F)); utf8[2] = (char)(0x80 | (cp & 0x3F)); len = 3; }
    else { utf8[0] = (char)(0xF0 | (cp >> 18)); utf8[1] = (char)(0x80 | ((cp >> 12) & 0x3F)); utf8[2] = (char)(0x80 | ((cp >> 6) & 0x3F)); utf8[3] = (char)(0x80 | (cp & 0x3F)); len = 4; }
    utf8[len] = '\0';
    ncplane_putstr_yx(_fun_nc_std, y, x, utf8);
    push_value(vm, make_int(0));
  } else {
    push_value(vm, make_int(-1));
  }
#else
  (void)cp; (void)x; (void)y;
  push_value(vm, make_int(-1));
#endif
  break;
}
