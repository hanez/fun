/**
 * Implements OP_ECHO: print top-of-stack value without trailing newline.
 * Now stores the value into the VM's output buffer and marks it as partial,
 * so the CLI can render echo output together with following print output.
 */

case OP_ECHO: {
    Value v = pop_value(vm);
    Value snap = deep_copy_value(&v);
    free_value(v);
    if (vm->output_count < OUTPUT_SIZE) {
        int idx = vm->output_count;
        vm->output[idx] = snap;
        vm->output_is_partial[idx] = 1; // ECHO does not end the line
        vm->output_count++;
    } else {
        free_value(snap);
        fprintf(stderr, "Runtime error: output buffer overflow\n");
        exit(1);
    }
    break;
}
