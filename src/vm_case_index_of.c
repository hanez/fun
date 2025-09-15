case OP_INDEX_OF: {
    Value needle = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: INDEX_OF expects (array, value)\n");
        exit(1);
    }
    int idx = array_index_of(&arr, &needle);
    free_value(arr);
    free_value(needle);
    push_value(vm, make_int(idx));
    break;
}
