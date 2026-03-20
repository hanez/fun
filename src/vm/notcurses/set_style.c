/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* NC_SET_STYLE */
case OP_NC_SET_STYLE: {
  Value fgv = pop_value(vm);
  Value bgv = pop_value(vm);
  Value stylev = pop_value(vm);
  int style = (stylev.type == VAL_INT ? (int)stylev.i : (stylev.type == VAL_FLOAT ? (int)stylev.d : 0));
  int bg = (bgv.type == VAL_INT ? (int)bgv.i : (bgv.type == VAL_FLOAT ? (int)bgv.d : 0));
  int fg = (fgv.type == VAL_INT ? (int)fgv.i : (fgv.type == VAL_FLOAT ? (int)fgv.d : 0));
  free_value(stylev);
  free_value(bgv);
  free_value(fgv);
#ifdef FUN_WITH_NOTCURSES
  if (_fun_nc && _fun_nc_std) {
    /* Extract RGB components */
    unsigned fg_r = (unsigned)((fg >> 16) & 0xFF);
    unsigned fg_g = (unsigned)((fg >> 8) & 0xFF);
    unsigned fg_b = (unsigned)(fg & 0xFF);
    unsigned bg_r = (unsigned)((bg >> 16) & 0xFF);
    unsigned bg_g = (unsigned)((bg >> 8) & 0xFF);
    unsigned bg_b = (unsigned)(bg & 0xFF);

    /* Set colors */
    ncplane_set_fg_rgb8(_fun_nc_std, fg_r, fg_g, fg_b);
    ncplane_set_bg_rgb8(_fun_nc_std, bg_r, bg_g, bg_b);

    /* Map simple bitmask to notcurses styles */
    /* some notcurses versions spell these differently; provide fallbacks */
    #ifndef NCSTYLE_DIM
    #  ifdef NCSTYLE_FAINT
    #    define NCSTYLE_DIM NCSTYLE_FAINT
    #  else
    #    define NCSTYLE_DIM 0
    #  endif
    #endif
    #ifndef NCSTYLE_REVERSE
    #  ifdef NCSTYLE_REVERSED
    #    define NCSTYLE_REVERSE NCSTYLE_REVERSED
    #  else
    #    define NCSTYLE_REVERSE 0
    #  endif
    #endif
    unsigned styles = 0;
    if (style & 1) styles |= NCSTYLE_BOLD;
    if (style & 2) styles |= NCSTYLE_DIM;
    if (style & 4) styles |= NCSTYLE_ITALIC;
    if (style & 8) styles |= NCSTYLE_UNDERLINE;
    if (style & 16) styles |= NCSTYLE_STRUCK;
    if (style & 32) styles |= NCSTYLE_REVERSE;
    ncplane_set_styles(_fun_nc_std, styles);

    push_value(vm, make_int(0));
  } else {
    push_value(vm, make_int(-1));
  }
#else
  (void)style;
  (void)bg;
  (void)fg;
  push_value(vm, make_int(-1));
#endif
  break;
}
