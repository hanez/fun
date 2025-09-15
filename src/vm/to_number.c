case OP_TO_NUMBER: {
    Value v = pop_value(vm);
    if (v.type == VAL_INT) {
        Value out = make_int(v.i);
        free_value(v);
        push_value(vm, out);
    } else if (v.type == VAL_STRING) {
        const char *s = v.s ? v.s : "";
        const char *p = s;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        char *endp = NULL;
        long long parsed = strtoll(p, &endp, 10);
        while (endp && (*endp == ' ' || *endp == '\t' || *endp == '\r' || *endp == '\n')) endp++;
        if (endp && *endp != '\0') {
            push_value(vm, make_int(0));
        } else {
            push_value(vm, make_int((int64_t)parsed));
        }
        free_value(v);
    } else {
        free_value(v);
        push_value(vm, make_int(0));
    }
    break;
}
