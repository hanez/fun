/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_TYPEOF: {
    Value v = pop_value(vm);
    const char *tname = "Unknown";
    switch (v.type) {
        case VAL_INT:      tname = "Number";   break;
        case VAL_STRING:   tname = "String";   break;
        case VAL_FUNCTION: tname = "Function"; break;
        case VAL_ARRAY:    tname = "Array";    break;
        case VAL_MAP:      tname = "Map";      break;
        case VAL_NIL:      tname = "Nil";      break;
        default:           tname = "Unknown";  break;
    }
    /* push a new string value; make_string duplicates the C string */
    push_value(vm, make_string(tname));
    free_value(v);
    break;
}
