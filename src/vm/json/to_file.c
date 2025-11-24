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

/* JSON_TO_FILE */
case OP_JSON_TO_FILE: {
#ifdef FUN_WITH_JSON
    Value vpretty = pop_value(vm);
    Value any = pop_value(vm);
    Value vpath = pop_value(vm);
    char *path = value_to_string_alloc(&vpath);
    int pretty = (vpretty.type == VAL_BOOL || vpretty.type == VAL_INT) ? (vpretty.i != 0) : 0;
    free_value(vpretty);
    free_value(vpath);
    if (!path) { free_value(any); push_value(vm, make_int(0)); break; }
    json_object *j = fun_to_json(&any);
    int flags = pretty ? JSON_C_TO_STRING_PRETTY : JSON_C_TO_STRING_PLAIN;
    int rc = json_object_to_file_ext(path, j, flags);
    json_object_put(j);
    free(path);
    free_value(any);
    push_value(vm, make_int(rc == 0 ? 1 : 0));
#else
    Value vpretty = pop_value(vm); free_value(vpretty);
    Value any = pop_value(vm); free_value(any);
    Value vpath = pop_value(vm); free_value(vpath);
    push_value(vm, make_int(0));
#endif
    break;
}
