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
 
 /* TK_EVAL */
case OP_TK_EVAL: {
    Value text = pop_value(vm);
    char *s = value_to_string_alloc(&text);
    free_value(text);
    int rc = fun_tk_eval_script(s ? s : "");
    if (s) free(s);
    push_value(vm, make_int(rc));
    break;
}
