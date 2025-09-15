case OP_PRINT: {
    Value v = pop_value(vm);
    /* snapshot value at print time (deep copy arrays) */
    Value snap = deep_copy_value(&v);
    free_value(v);
    if (vm->output_count < VM_OUTPUT_SIZE) {
        vm->output[vm->output_count++] = snap;  // store snapshot
    } else {
        free_value(snap);  // prevent leak
        fprintf(stderr, "Runtime error: output buffer overflow\n");
        exit(1);
    }
    break;
}
