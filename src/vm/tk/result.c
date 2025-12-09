/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

/* TK_RESULT */
case OP_TK_RESULT: {
    const char *r = fun_tk_get_result();
    push_value(vm, make_string(r ? r : ""));
    break;
}
