case OP_MIN: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) { fprintf(stderr, "MIN expects ints\n"); exit(1); }
    push_value(vm, make_int(a.i < b.i ? a.i : b.i));
    free_value(a); free_value(b);
    break;
}
