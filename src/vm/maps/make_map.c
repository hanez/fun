case OP_MAKE_MAP: {
    int pairs = inst.operand;
    if (pairs < 0) { fprintf(stderr, "MAKE_MAP invalid pair count\n"); exit(1); }
    Value m = make_map_empty();
    for (int i = 0; i < pairs; ++i) {
        Value val = pop_value(vm);
        Value key = pop_value(vm);
        if (key.type != VAL_STRING) { fprintf(stderr, "Map literal keys must be strings\n"); exit(1); }
        if (!map_set(&m, key.s ? key.s : "", val)) {
            fprintf(stderr, "Map literal set failed\n"); exit(1);
        }
        free_value(key);
    }
    push_value(vm, m);
    break;
}
