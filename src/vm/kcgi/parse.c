/* KCGI_PARSE */
case OP_KCGI_PARSE: {
#ifdef FUN_WITH_KCGI
  if (g_kcgi_req) { /* safety: free previous if any */
    kcgi_free_request(g_kcgi_req);
    g_kcgi_req = NULL;
  }
  struct kreq *r = NULL;
  if (!kcgi_parse_request(&r)) {
    push_value(vm, make_nil());
    break;
  }
  g_kcgi_req = r;
  Value v = kreq_to_fun(r);
  push_value(vm, v);
#else
  push_value(vm, make_nil());
#endif
  break;
}
