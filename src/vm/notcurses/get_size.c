/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* NC_GET_SIZE */
case OP_NC_GET_SIZE: {
#ifdef FUN_WITH_NOTCURSES
  if (_fun_nc_std) {
    int rows = 0, cols = 0;
    ncplane_dim_yx(_fun_nc_std, &rows, &cols);
    Value tmp[2];
    tmp[0] = make_int(rows);
    tmp[1] = make_int(cols);
    Value arr = make_array_from_values(tmp, 2);
    push_value(vm, arr);
  } else {
    push_value(vm, make_int(-1));
  }
#else
  (void)_fun_nc_std;
  push_value(vm, make_int(-1));
#endif
  break;
}
