case OP_CLAMP: {
    Value hi = pop_value(vm);
    Value lo = pop_value(vm);
    Value x = pop_value(vm);
    if (x.type != VAL_INT || lo.type != VAL_INT || hi.type != VAL_INT) {
        fprintf(stderr, "CLAMP expects ints\n");
        exit(1);
    }
    int64_t v = x.i;
    if (v < lo.i) v = lo.i;
    if (v > hi.i) v = hi.i;
    push_value(vm, make_int(v));
    free_value(x);
    free_value(lo);
    free_value(hi);
    break;
}
