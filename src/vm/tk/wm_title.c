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

/* TK_WM_TITLE */
case OP_TK_WM_TITLE: {
    Value titlev = pop_value(vm);
    char *title = value_to_string_alloc(&titlev);
    free_value(titlev);
    if (!title) { push_value(vm, make_int(-1)); break; }
    /* Escape backslashes and double quotes for Tcl double-quoted strings */
    size_t n = 0; for (const char *p = title; *p; ++p) { n += (*p == '\\' || *p == '"') ? 2 : 1; }
    char *esc = (char*)malloc(n + 1);
    if (!esc) { free(title); push_value(vm, make_int(-1)); break; }
    char *q = esc; for (const char *p = title; *p; ++p) { if (*p == '\\' || *p == '"') *q++ = '\\'; *q++ = *p; } *q = '\0';
    free(title);
    size_t slen = strlen(esc) + 32;
    char *script = (char*)malloc(slen);
    if (!script) { free(esc); push_value(vm, make_int(-1)); break; }
    snprintf(script, slen, "wm title . \"%s\"", esc);
    int rc = fun_tk_eval_script(script);
    free(script);
    free(esc);
    push_value(vm, make_int(rc));
    break;
}
