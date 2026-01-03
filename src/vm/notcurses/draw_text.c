/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-03
 */

/* NC_DRAW_TEXT */
case OP_NC_DRAW_TEXT: {
    Value textv = pop_value(vm);
    Value xv = pop_value(vm);
    Value yv = pop_value(vm);
    int x = (xv.type == VAL_INT ? (int)xv.i : (xv.type == VAL_FLOAT ? (int)xv.d : 0));
    int y = (yv.type == VAL_INT ? (int)yv.i : (yv.type == VAL_FLOAT ? (int)yv.d : 0));
    char *text = value_to_string_alloc(&textv);
    free_value(textv);
    free_value(xv);
    free_value(yv);
#ifdef FUN_WITH_NOTCURSES
    if (_fun_nc && _fun_nc_std && text) {
        ncplane_putstr_yx(_fun_nc_std, y, x, text);
        notcurses_render(_fun_nc);
        free(text);
        push_value(vm, make_int(0));
    } else {
        if (text) free(text);
        push_value(vm, make_int(-1));
    }
#else
    (void)_fun_nc; (void)_fun_nc_std;
    if (text) free(text);
    push_value(vm, make_int(-1));
#endif
    break;
}
