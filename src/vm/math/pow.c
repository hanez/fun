case OP_POW: {
    Value b = pop_value(vm);
    Value a = pop_value(vm);
    if (a.type != VAL_INT || b.type != VAL_INT) { fprintf(stderr, "POW expects ints\n"); exit(1); }
    int64_t base = a.i;
    int64_t exp = b.i;
    int64_t res = 1;
    if (exp < 0) { res = 0; } else {
        while (exp > 0) {
            if (exp & 1) res *= base;
            base *= base;
            exp >>= 1;
        }
    }
    push_value(vm, make_int(res));
    free_value(a); free_value(b);
    break;
}
