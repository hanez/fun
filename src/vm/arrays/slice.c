case OP_SLICE: {
    Value end = pop_value(vm);
    Value start = pop_value(vm);
    Value arr = pop_value(vm);
    if (arr.type != VAL_ARRAY || start.type != VAL_INT || end.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: SLICE expects (array, int, int)\n");
        exit(1);
    }
    Value out = array_slice(&arr, (int)start.i, (int)end.i);
    free_value(arr);
    free_value(start);
    free_value(end);
    push_value(vm, out);
    break;
}
