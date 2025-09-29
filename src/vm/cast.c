/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
 * @file cast.c
 * @brief Implements the OP_CAST opcode for converting a value to a target type.
 *
 * Stack contract:
 * - Pops: typeName (String), value (any)
 * - Pushes: converted value (or Nil on unsupported target)
 */

case OP_CAST: {
    /* pop type then value (args pushed in this order: value, typeName) */
    Value t = pop_value(vm);
    Value v = pop_value(vm);

    const char *tn = (t.type == VAL_STRING && t.s) ? t.s : NULL;
    Value out = make_nil();

    /* Normalize target name to lowercase into a small buffer */
    char target[32];
    int k = 0;
    if (tn) {
        const char *p = tn;
        while (*p && k < (int)sizeof(target) - 1) {
            char c = *p++;
            if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
            target[k++] = c;
        }
    }
    target[k] = '\0';

    if (!tn) {
        out = make_nil();
    } else if (strcmp(target, "number") == 0) {
        if (v.type == VAL_INT) {
            out = make_int(v.i);
        } else if (v.type == VAL_STRING) {
            const char *s = v.s ? v.s : "";
            const char *p = s;
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
            char *endp = NULL;
            long long parsed = strtoll(p, &endp, 10);
            while (endp && (*endp == ' ' || *endp == '\t' || *endp == '\r' || *endp == '\n')) endp++;
            if (endp && *endp != '\0') out = make_int(0);
            else out = make_int((int64_t)parsed);
        } else {
            out = make_int(0);
        }
    } else if (strcmp(target, "string") == 0) {
        char *s = value_to_string_alloc(&v);
        out = make_string(s ? s : "");
        if (s) free(s);
    } else if (strcmp(target, "array") == 0) {
        if (v.type == VAL_ARRAY) {
            out = copy_value(&v);
        } else {
            Value tmp = deep_copy_value(&v);
            out = make_array_from_values(&tmp, 1);
            free_value(tmp);
        }
    } else if (strcmp(target, "map") == 0) {
        if (v.type == VAL_MAP) {
            out = copy_value(&v);
        } else {
            out = make_map_empty();
        }
    } else if (strcmp(target, "nil") == 0) {
        out = make_nil();
    } else if (strcmp(target, "function") == 0) {
        out = (v.type == VAL_FUNCTION) ? copy_value(&v) : make_nil();
    } else if (strcmp(target, "boolean") == 0) {
        out = make_int(value_is_truthy(&v) ? 1 : 0);
    } else {
        out = make_nil();
    }

    free_value(t);
    free_value(v);
    push_value(vm, out);
    break;
}
