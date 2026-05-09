/* KCGI_END */
case OP_KCGI_END: {
#ifdef FUN_WITH_KCGI
  if (g_kcgi_req) { kcgi_free_request(g_kcgi_req); g_kcgi_req = NULL; }
  push_value(vm, make_int(1));
#else
  push_value(vm, make_int(0));
#endif
  break;
}
