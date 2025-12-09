/**
 * Implements OP_ECHO: print top-of-stack value without trailing newline.
 * Does not store into VM output buffer; writes directly to stdout and flushes.
 */

case OP_ECHO: {
    Value v = pop_value(vm);
    print_value(&v);
    fflush(stdout);
    free_value(v);
    break;
}
