case OP_INDEX_SET: {
    Value v = pop_value(vm);
    Value idx = pop_value(vm);
    Value container = pop_value(vm);
    if (container.type == VAL_ARRAY) {
        if (idx.type != VAL_INT) { fprintf(stderr, "INDEX_SET index must be int for array\n"); exit(1); }
        if (!array_set(&container, (int)idx.i, v)) {
            fprintf(stderr, "Runtime error: index out of range\n"); exit(1);
        }
        /* v moved into array */
        free_value(container);
        free_value(idx);
    } else if (container.type == VAL_MAP) {
        if (idx.type != VAL_STRING) { fprintf(stderr, "INDEX_SET key must be string for map\n"); exit(1); }
        if (!map_set(&container, idx.s ? idx.s : "", v)) {
            fprintf(stderr, "Runtime error: map set failed\n"); exit(1);
        }
        free_value(container);
        free_value(idx);
    } else {
        fprintf(stderr, "Runtime type error: INDEX_SET expects array or map\n");
        exit(1);
    }
    break;
}
