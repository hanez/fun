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

/* NC_INIT */
case OP_NC_INIT: {
#ifdef FUN_WITH_NOTCURSES
    if (_fun_nc) { push_value(vm, make_int(1)); break; }
    struct notcurses_options opts = {0};
    _fun_nc = notcurses_core_init(&opts, NULL);
    if (!_fun_nc) { push_value(vm, make_int(0)); break; }
    _fun_nc_std = notcurses_stdplane(_fun_nc);
    push_value(vm, make_int(1));
#else
    (void)_fun_nc; (void)_fun_nc_std;
    fprintf(stderr, "Notcurses support disabled at build time. Reconfigure with -DFUN_WITH_NOTCURSES=ON.\n");
    push_value(vm, make_int(0));
#endif
    break;
}
