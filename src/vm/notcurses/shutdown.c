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

/* NC_SHUTDOWN */
case OP_NC_SHUTDOWN: {
#ifdef FUN_WITH_NOTCURSES
    if (_fun_nc) {
        notcurses_stop(_fun_nc);
        _fun_nc = NULL;
        _fun_nc_std = NULL;
    }
#else
    (void)_fun_nc; (void)_fun_nc_std;
#endif
    push_value(vm, make_int(0));
    break;
}
