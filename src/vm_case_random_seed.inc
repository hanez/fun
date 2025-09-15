case OP_RANDOM_SEED: {
    Value seed = pop_value(vm);
    if (seed.type != VAL_INT) { fprintf(stderr, "RANDOM_SEED expects int\n"); exit(1); }
    srand((unsigned int)seed.i);
    free_value(seed);
    push_value(vm, make_int(0));
    break;
}
