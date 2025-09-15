case OP_ABS: {
    Value x = pop_value(vm);
    if (x.type != VAL_INT) { fprintf(stderr, "ABS expects int\n"); exit(1); }
    int64_t v = x.i;
    if (v < 0) v = -v;
    push_value(vm, make_int(v));
    free_value(x);
    break;
}
