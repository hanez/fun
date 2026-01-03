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

/* NC_CLEAR */
case OP_NC_CLEAR: {
#ifdef FUN_WITH_NOTCURSES
    if (_fun_nc && _fun_nc_std) {
        ncplane_erase(_fun_nc_std);
        notcurses_render(_fun_nc);
        push_value(vm, make_int(0));
    } else {
        push_value(vm, make_int(-1));
    }
#else
    (void)_fun_nc; (void)_fun_nc_std;
    push_value(vm, make_int(-1));
#endif
    break;
}
