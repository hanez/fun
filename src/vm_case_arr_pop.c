case OP_ARR_POP: {
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY) {
        fprintf(stderr, "Runtime type error: ARR_POP expects array\n");
        exit(1);
    }
    Value out;
    if (!array_pop(&arr, &out)) {
        fprintf(stderr, "Runtime error: pop from empty array\n");
        exit(1);
    }
    free_value(arr);
    push_value(vm, out);
    break;
}
