/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-30
 */

/* OP_INI_FREE: pops handle; pushes 1/0 */
#ifdef FUN_WITH_INI
case OP_INI_FREE: {
    Value vh = pop_value(vm);
    int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
    free_value(vh);
    int ok = ini_free_handle(h);
    push_value(vm, make_int(ok));
    break;
}
#endif
