case OP_EQ: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int eq = 0;
    if (a.type == b.type) {
        switch (a.type) {
            case VAL_INT:      eq = (a.i == b.i); break;
            case VAL_STRING:   eq = (a.s && b.s) ? (strcmp(a.s, b.s) == 0) : (a.s == b.s); break;
            case VAL_FUNCTION: eq = (a.fn == b.fn); break;
            case VAL_NIL:      eq = 1; break;
            default:           eq = 0; break;
        }
    } else {
        eq = 0;
    }
    push_value(vm, make_int(eq ? 1 : 0));
    free_value(a); free_value(b);
    break;
}
