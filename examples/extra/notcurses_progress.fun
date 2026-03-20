#!/usr/bin/env fun

include <ui/notcurses.fun>

n = Notcurses()
if n.init() == 0
  print("Notcurses not available. Build with -DFUN_WITH_NOTCURSES=ON.")
  exit(1)

n.clear()
sz = n.size()
rows = 24
cols = 80
if typeof(sz) == "array"
  if len(sz) >= 2
    rows = sz[0]
    cols = sz[1]

y = to_number(rows) / 2
w = cols - 10
if w < 20
  w = 20

i = 0
while i <= 100
  // styles handled inside progress(); keep plane sane
  rc = n.set_style(0x00FF00, 0x000000, 1)
  frac = to_number(i) / 100
  rc = n.progress(y, 5, w, frac, 0)
  k = n.getch(10)
  i = i + 1

// final message
rc = n.set_style(0xFFFFFF, 0x000000, 1)
rc = n.draw_text(y + 2, 5, "Done!")
rc = n.render()

rc = n.shutdown()
