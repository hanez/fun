case OP_VALUES: {
    Value m = pop_value(vm);
    if (m.type != VAL_MAP) { fprintf(stderr, "VALUES expects map\n"); exit(1); }
    Value arr = map_values_array(&m);
    free_value(m);
    push_value(vm, arr);
    break;
}
