case OP_INDEX_GET: {
    Value idx = pop_value(vm);
    Value container = pop_value(vm);
#ifdef FUN_DEBUG
    fprintf(stderr, "DEBUG INDEX_GET: container.type=%d idx.type=%d\n",
            container.type, idx.type);
#endif
    if (container.type == VAL_ARRAY) {
        if (idx.type != VAL_INT) { fprintf(stderr, "INDEX_GET index must be int for array\n"); exit(1); }
        Value elem;
        if (!array_get_copy(&container, (int)idx.i, &elem)) {
            fprintf(stderr, "Runtime error: index out of range\n"); exit(1);
        }
        free_value(container);
        free_value(idx);
        push_value(vm, elem);
    } else if (container.type == VAL_MAP) {
        if (idx.type != VAL_STRING) { fprintf(stderr, "INDEX_GET key must be string for map\n"); exit(1); }
        Value out;
        if (!map_get_copy(&container, idx.s ? idx.s : "", &out)) {
            out = make_nil();
        }
        free_value(container);
        free_value(idx);
        push_value(vm, out);
    } else {
        fprintf(stderr, "Runtime type error: INDEX_GET expects array or map\n");
        exit(1);
    }
    break;
}
