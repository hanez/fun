/* KCGI_REPLY_START */
case OP_KCGI_REPLY_START: {
#ifdef FUN_WITH_KCGI
  Value vct = pop_value(vm);
  Value vcode = pop_value(vm);
  int code = 200;
  if (vcode.type == VAL_INT) {
    code = (int)vcode.i;
  } else if (vcode.type == VAL_FLOAT) {
    code = (int)vcode.d;
  } else if (vcode.type == VAL_STRING) {
    if (vcode.s) code = atoi(vcode.s);
  }
  char *ct = value_to_string_alloc(&vct);
  free_value(vcode);
  free_value(vct);
  if (!ct || ct[0] == '\0') {
    /* default content type */
    free(ct);
    ct = strdup("text/html; charset=utf-8");
  }
  int ok = kcgi_reply_start(code, ct);
  free(ct);
  push_value(vm, make_int(ok ? 1 : 0));
#else
  Value a = pop_value(vm); free_value(a);
  Value b = pop_value(vm); free_value(b);
  push_value(vm, make_int(0));
#endif
  break;
}
