/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/* NC_BOX */
case OP_NC_BOX: {
  Value stylev = pop_value(vm);
  Value hv = pop_value(vm);
  Value wv = pop_value(vm);
  Value yv = pop_value(vm);
  Value xv = pop_value(vm);
  int style = (stylev.type == VAL_INT ? (int)stylev.i : (stylev.type == VAL_FLOAT ? (int)stylev.d : 0));
  int h = (hv.type == VAL_INT ? (int)hv.i : (hv.type == VAL_FLOAT ? (int)hv.d : 0));
  int w = (wv.type == VAL_INT ? (int)wv.i : (wv.type == VAL_FLOAT ? (int)wv.d : 0));
  int y = (yv.type == VAL_INT ? (int)yv.i : (yv.type == VAL_FLOAT ? (int)yv.d : 0));
  int x = (xv.type == VAL_INT ? (int)xv.i : (xv.type == VAL_FLOAT ? (int)xv.d : 0));
  free_value(stylev); free_value(hv); free_value(wv); free_value(yv); free_value(xv);
#ifdef FUN_WITH_NOTCURSES
  if (_fun_nc && _fun_nc_std && h >= 2 && w >= 2) {
    /* Pick characters: style 0 light (ASCII), 1 heavy (Unicode), fallback to ASCII */
    uint32_t hch = (style == 1 ? 0x2501 : '-'); // heavy box drawings horizontal
    uint32_t vch = (style == 1 ? 0x2503 : '|'); // heavy vertical
    uint32_t tl = (style == 1 ? 0x250F : '+');
    uint32_t tr = (style == 1 ? 0x2513 : '+');
    uint32_t bl = (style == 1 ? 0x2517 : '+');
    uint32_t br = (style == 1 ? 0x251B : '+');

    /* Draw edges */
    // top and bottom
    {
      char topc[5]; int tlen=0; uint32_t cp=hch;
      if (cp<=0x7F){topc[0]=(char)cp;tlen=1;} else if (cp<=0x7FF){topc[0]=(char)(0xC0|(cp>>6)); topc[1]=(char)(0x80|(cp&0x3F)); tlen=2;} else if (cp<=0xFFFF){topc[0]=(char)(0xE0|(cp>>12)); topc[1]=(char)(0x80|((cp>>6)&0x3F)); topc[2]=(char)(0x80|(cp&0x3F)); tlen=3;} else {topc[0]=(char)(0xF0|(cp>>18)); topc[1]=(char)(0x80|((cp>>12)&0x3F)); topc[2]=(char)(0x80|((cp>>6)&0x3F)); topc[3]=(char)(0x80|(cp&0x3F)); tlen=4;}
      topc[tlen]='\0';
      for (int i=1;i<w-1;i++){ ncplane_putstr_yx(_fun_nc_std, y, x+i, topc); ncplane_putstr_yx(_fun_nc_std, y+h-1, x+i, topc);}    
    }
    // left and right
    {
      char vertc[5]; int vlen=0; uint32_t cp=vch;
      if (cp<=0x7F){vertc[0]=(char)cp;vlen=1;} else if (cp<=0x7FF){vertc[0]=(char)(0xC0|(cp>>6)); vertc[1]=(char)(0x80|(cp&0x3F)); vlen=2;} else if (cp<=0xFFFF){vertc[0]=(char)(0xE0|(cp>>12)); vertc[1]=(char)(0x80|((cp>>6)&0x3F)); vertc[2]=(char)(0x80|(cp&0x3F)); vlen=3;} else {vertc[0]=(char)(0xF0|(cp>>18)); vertc[1]=(char)(0x80|((cp>>12)&0x3F)); vertc[2]=(char)(0x80|((cp>>6)&0x3F)); vertc[3]=(char)(0x80|(cp&0x3F)); vlen=4;}
      vertc[vlen]='\0';
      for (int i=1;i<h-1;i++){ ncplane_putstr_yx(_fun_nc_std, y+i, x, vertc); ncplane_putstr_yx(_fun_nc_std, y+i, x+w-1, vertc);}    
    }
    // corners
    {
      char cbuf[5]; int clen=0; uint32_t cp;
      cp=tl; if (cp<=0x7F){cbuf[0]=(char)cp;clen=1;} else if (cp<=0x7FF){cbuf[0]=(char)(0xC0|(cp>>6)); cbuf[1]=(char)(0x80|(cp&0x3F)); clen=2;} else if (cp<=0xFFFF){cbuf[0]=(char)(0xE0|(cp>>12)); cbuf[1]=(char)(0x80|((cp>>6)&0x3F)); cbuf[2]=(char)(0x80|(cp&0x3F)); clen=3;} else {cbuf[0]=(char)(0xF0|(cp>>18)); cbuf[1]=(char)(0x80|((cp>>12)&0x3F)); cbuf[2]=(char)(0x80|((cp>>6)&0x3F)); cbuf[3]=(char)(0x80|(cp&0x3F)); clen=4;} cbuf[clen]='\0'; ncplane_putstr_yx(_fun_nc_std, y, x, cbuf);
      cp=tr; if (cp<=0x7F){cbuf[0]=(char)cp;clen=1;} else if (cp<=0x7FF){cbuf[0]=(char)(0xC0|(cp>>6)); cbuf[1]=(char)(0x80|(cp&0x3F)); clen=2;} else if (cp<=0xFFFF){cbuf[0]=(char)(0xE0|(cp>>12)); cbuf[1]=(char)(0x80|((cp>>6)&0x3F)); cbuf[2]=(char)(0x80|(cp&0x3F)); clen=3;} else {cbuf[0]=(char)(0xF0|(cp>>18)); cbuf[1]=(char)(0x80|((cp>>12)&0x3F)); cbuf[2]=(char)(0x80|((cp>>6)&0x3F)); cbuf[3]=(char)(0x80|(cp&0x3F)); clen=4;} cbuf[clen]='\0'; ncplane_putstr_yx(_fun_nc_std, y, x+w-1, cbuf);
      cp=bl; if (cp<=0x7F){cbuf[0]=(char)cp;clen=1;} else if (cp<=0x7FF){cbuf[0]=(char)(0xC0|(cp>>6)); cbuf[1]=(char)(0x80|(cp&0x3F)); clen=2;} else if (cp<=0xFFFF){cbuf[0]=(char)(0xE0|(cp>>12)); cbuf[1]=(char)(0x80|((cp>>6)&0x3F)); cbuf[2]=(char)(0x80|(cp&0x3F)); clen=3;} else {cbuf[0]=(char)(0xF0|(cp>>18)); cbuf[1]=(char)(0x80|((cp>>12)&0x3F)); cbuf[2]=(char)(0x80|((cp>>6)&0x3F)); cbuf[3]=(char)(0x80|(cp&0x3F)); clen=4;} cbuf[clen]='\0'; ncplane_putstr_yx(_fun_nc_std, y+h-1, x, cbuf);
      cp=br; if (cp<=0x7F){cbuf[0]=(char)cp;clen=1;} else if (cp<=0x7FF){cbuf[0]=(char)(0xC0|(cp>>6)); cbuf[1]=(char)(0x80|(cp&0x3F)); clen=2;} else if (cp<=0xFFFF){cbuf[0]=(char)(0xE0|(cp>>12)); cbuf[1]=(char)(0x80|((cp>>6)&0x3F)); cbuf[2]=(char)(0x80|(cp&0x3F)); clen=3;} else {cbuf[0]=(char)(0xF0|(cp>>18)); cbuf[1]=(char)(0x80|((cp>>12)&0x3F)); cbuf[2]=(char)(0x80|((cp>>6)&0x3F)); cbuf[3]=(char)(0x80|(cp&0x3F)); clen=4;} cbuf[clen]='\0'; ncplane_putstr_yx(_fun_nc_std, y+h-1, x+w-1, cbuf);
    }

    push_value(vm, make_int(0));
  } else {
    push_value(vm, make_int(-1));
  }
#else
  (void)style; (void)h; (void)w; (void)y; (void)x;
  push_value(vm, make_int(-1));
#endif
  break;
}
