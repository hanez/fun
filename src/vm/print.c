case OP_PRINT: {
    Value v = pop_value(vm);
    Value snap = deep_copy_value(&v);
    free_value(v);
    if (vm->output_count < VM_OUTPUT_SIZE) {
        vm->output[vm->output_count++] = snap;
    } else {
        free_value(snap);
        fprintf(stderr, "Runtime error: output buffer overflow\n");
        exit(1);
    }
    break;
}
