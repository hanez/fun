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

/* JSON_FROM_FILE */
case OP_JSON_FROM_FILE: {
#ifdef FUN_WITH_JSON
    Value vpath = pop_value(vm);
    char *path = value_to_string_alloc(&vpath);
    free_value(vpath);
    if (!path) { push_value(vm, make_nil()); break; }
    json_object *root = json_object_from_file(path);
    free(path);
    if (!root) { push_value(vm, make_nil()); break; }
    Value v = json_to_fun(root);
    push_value(vm, v);
    json_object_put(root);
#else
    Value vpath = pop_value(vm); free_value(vpath);
    push_value(vm, make_nil());
#endif
    break;
}
