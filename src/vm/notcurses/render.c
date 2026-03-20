/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* NC_RENDER */
case OP_NC_RENDER: {
#ifdef FUN_WITH_NOTCURSES
  if (_fun_nc) {
    int rc = notcurses_render(_fun_nc);
    push_value(vm, make_int(rc == 0 ? 0 : -1));
  } else {
    push_value(vm, make_int(-1));
  }
#else
  push_value(vm, make_int(-1));
#endif
  break;
}
