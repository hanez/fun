case OP_TYPEOF: {
    Value v = pop_value(vm);
    const char *tname = "Unknown";
    switch (v.type) {
        case VAL_INT:      tname = "Number";   break;
        case VAL_STRING:   tname = "String";   break;
        case VAL_FUNCTION: tname = "Function"; break;
        case VAL_ARRAY:    tname = "Array";    break;
        case VAL_MAP:      tname = "Map";      break;
        case VAL_NIL:      tname = "Nil";      break;
        default:           tname = "Unknown";  break;
    }
    /* push a new string value; make_string duplicates the C string */
    push_value(vm, make_string(tname));
    free_value(v);
    break;
}