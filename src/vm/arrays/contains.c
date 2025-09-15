case OP_CONTAINS: {
    Value needle = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: CONTAINS expects (array, value)\n");
        exit(1);
    }
    int ok = array_contains(&arr, &needle);
    free_value(arr);
    free_value(needle);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
