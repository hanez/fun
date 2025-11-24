/**
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-11-24
 */

/* JSON_PARSE */
case OP_JSON_PARSE: {
#ifdef FUN_WITH_JSON
    Value text = pop_value(vm);
    char *s = value_to_string_alloc(&text);
    free_value(text);
    if (!s) { push_value(vm, make_nil()); break; }
    struct json_tokener *tok = json_tokener_new();
    json_object *root = json_tokener_parse_ex(tok, s, (int)strlen(s));
    enum json_tokener_error jerr = json_tokener_get_error(tok);
    json_tokener_free(tok);
    free(s);
    if (jerr != json_tokener_success) {
        push_value(vm, make_nil());
    } else {
        Value v = json_to_fun(root);
        push_value(vm, v);
        json_object_put(root);
    }
#else
    /* Fallback when JSON is disabled: consume arg, push Nil */
    Value drop = pop_value(vm);
    free_value(drop);
    push_value(vm, make_nil());
#endif
    break;
}
