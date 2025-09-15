case OP_HAS_KEY: {
    Value key = pop_value(vm);
    Value m = pop_value(vm);
    if (m.type != VAL_MAP || key.type != VAL_STRING) { fprintf(stderr, "HAS_KEY expects (map, string)\n"); exit(1); }
    int ok = map_has(&m, key.s ? key.s : "");
    free_value(m); free_value(key);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
