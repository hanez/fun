case OP_NEQ: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    int neq = 1;
    if (a.type == b.type) {
        switch (a.type) {
            case VAL_INT:      neq = (a.i != b.i); break;
            case VAL_STRING:   neq = (a.s && b.s) ? (strcmp(a.s, b.s) != 0) : (a.s != b.s); break;
            case VAL_FUNCTION: neq = (a.fn != b.fn); break;
            case VAL_NIL:      neq = 0; break;
            default:           neq = 1; break;
        }
    } else {
        neq = 1;
    }
    push_value(vm, make_int(neq ? 1 : 0));
    free_value(a); free_value(b);
    break;
}
