/**
 * OpenSSL MD5 builtin
 */
case OP_OPENSSL_MD5: {
    Value vdata = pop_value(vm);
    char *s = value_to_string_alloc(&vdata);
    free_value(vdata);
    if (!s) { push_value(vm, make_string("")); break; }
    char *hex = fun_openssl_md5_hex((const unsigned char*)s, strlen(s));
    free(s);
    if (!hex) { push_value(vm, make_string("")); break; }
    Value out = make_string(hex);
    free(hex);
    push_value(vm, out);
    break;
}
