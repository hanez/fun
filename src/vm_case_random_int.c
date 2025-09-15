case OP_RANDOM_INT: {
    Value hi = pop_value(vm);
    Value lo = pop_value(vm);
    if (lo.type != VAL_INT || hi.type != VAL_INT) { fprintf(stderr, "RANDOM_INT expects (int, int)\n"); exit(1); }
    int64_t a = lo.i, b = hi.i;
    if (b <= a) { push_value(vm, make_int((int64_t)a)); free_value(lo); free_value(hi); break; }
    int64_t span = b - a;
    int64_t r = (int64_t)(rand() % (span));
    push_value(vm, make_int(a + r));
    free_value(lo); free_value(hi);
    break;
}
