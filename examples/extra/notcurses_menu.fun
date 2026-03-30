#!/usr/bin/env fun

include <ui/notcurses.fun>

n = Notcurses()
if n.init() == 0
  print("Notcurses not available. Build with -DFUN_WITH_NOTCURSES=ON.")
  exit(1)

items = ["First", "Second", "Third", "Fourth"]
sel = 0
while true
  n.clear()
  // Title
  n.set_style(0xFFFF00, 0x000000, 1) // bold yellow
  n.draw_text(1, 2, "Demo Menu (j/k to move, Enter to select, ESC to quit)")

  // Render menu list
  n.menu(3, 4, items, sel, 30, 0, 32)

  key = n.getch(0)
  if key == 27 // ESC
    break
  if key == 106 // 'j'
    sel = (sel + 1) % len(items)
  if key == 107 // 'k'
    sel = (sel - 1 + len(items)) % len(items)
  if key == 10  // Enter
    n.status_bar("Selected: " + items[sel], 32)
    // brief wait
    sleep_ms(300)

n.shutdown()
