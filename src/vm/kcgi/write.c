/* KCGI_WRITE */
case OP_KCGI_WRITE: {
#ifdef FUN_WITH_KCGI
  Value vs = pop_value(vm);
  char *s = value_to_string_alloc(&vs);
  free_value(vs);
  int ok = kcgi_write_str(s ? s : "");
  if (s) free(s);
  push_value(vm, make_int(ok ? 1 : 0));
#else
  Value drop = pop_value(vm); free_value(drop);
  push_value(vm, make_int(0));
#endif
  break;
}
