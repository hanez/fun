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

/* JSON_STRINGIFY */
case OP_JSON_STRINGIFY: {
#ifdef FUN_WITH_JSON
    Value vpretty = pop_value(vm);
    Value any = pop_value(vm);
    int pretty = (vpretty.type == VAL_BOOL || vpretty.type == VAL_INT) ? (vpretty.i != 0) : 0;
    json_object *j = fun_to_json(&any);
    int flags = pretty ? JSON_C_TO_STRING_PRETTY : JSON_C_TO_STRING_PLAIN;
    const char *js = json_object_to_json_string_ext(j, flags);
    push_value(vm, make_string(js ? js : ""));
    json_object_put(j);
    free_value(vpretty);
    free_value(any);
#else
    /* Fallback: consume two args, push "null" */
    Value vpretty = pop_value(vm); free_value(vpretty);
    Value any = pop_value(vm); free_value(any);
    push_value(vm, make_string("null"));
#endif
    break;
}
