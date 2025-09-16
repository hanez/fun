case OP_SCLAMP: {
    /* Clamp/wrap to signed N-bit range: [-2^(N-1), 2^(N-1)-1] */
    Value v = pop_value(vm);
    int bits = inst.operand;
    uint64_t u = 0;

    if (v.type == VAL_INT) {
        u = (uint64_t)v.i;
    } else {
        u = 0;
    }

    if (bits <= 0) {
        /* treat as no-op for invalid widths */
    } else if (bits >= 64) {
        /* 64-bit signed: nothing to mask, keep as is */
    } else {
        uint64_t mask = (1ULL << bits) - 1ULL;
        u &= mask;
        uint64_t sign_bit = 1ULL << (bits - 1);
        if (u & sign_bit) {
            /* negative value in two's complement */
            u -= (1ULL << bits);
        }
    }

    push_value(vm, make_int((int64_t)u));
    free_value(v);
    break;
}
