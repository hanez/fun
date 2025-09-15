case OP_KEYS: {
    Value m = pop_value(vm);
    if (m.type != VAL_MAP) { fprintf(stderr, "KEYS expects map\n"); exit(1); }
    Value arr = map_keys_array(&m);
    free_value(m);
    push_value(vm, arr);
    break;
}
