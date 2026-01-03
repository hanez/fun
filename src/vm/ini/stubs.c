/*
 * This file provides stub handlers for INI opcodes when FUN_WITH_INI is disabled.
 * Each opcode reports a clear runtime error and returns a safe default.
 */

/* OP_INI_LOAD: pops path string; pushes 0 (invalid handle) */
case OP_INI_LOAD: {
    Value vpath = pop_value(vm);
    (void)vpath; /* unused */
    free_value(vpath);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    push_value(vm, make_int(0));
    break;
}

/* OP_INI_FREE: pops handle; pushes 0 */
case OP_INI_FREE: {
    Value vh = pop_value(vm);
    free_value(vh);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    push_value(vm, make_int(0));
    break;
}

/* Getters: pop args; push defaults (string:"", int:0, double:0.0, bool:0) */
case OP_INI_GET_STRING: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey);
    /* cannot convert here; return empty string */
    push_value(vm, make_string(""));
    free_value(vdef);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    break;
}

case OP_INI_GET_INT: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey);
    (void)vdef; /* unused */
    push_value(vm, make_int(0));
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    break;
}

case OP_INI_GET_DOUBLE: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey);
    (void)vdef; /* unused */
    push_value(vm, make_float(0.0));
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    break;
}

case OP_INI_GET_BOOL: {
    Value vdef = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey);
    (void)vdef; /* unused */
    push_value(vm, make_int(0));
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    break;
}

/* Mutators: return 0 */
case OP_INI_SET: {
    Value vval = pop_value(vm);
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey); free_value(vval);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    push_value(vm, make_int(0));
    break;
}

case OP_INI_UNSET: {
    Value vkey = pop_value(vm);
    Value vsec = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vsec); free_value(vkey);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    push_value(vm, make_int(0));
    break;
}

case OP_INI_SAVE: {
    Value vpath = pop_value(vm);
    Value vh = pop_value(vm);
    free_value(vh); free_value(vpath);
    fun_vm_fprintf(stderr, "Runtime error: INI support disabled (rebuild with -DFUN_WITH_INI=ON)\n");
    push_value(vm, make_int(0));
    break;
}
