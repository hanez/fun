# Notcurses (TUI) extension (optional)

- CMake option: FUN_WITH_NOTCURSES=ON
- Purpose: Terminal UI capabilities via the Notcurses library.
- Homepage: https://notcurses.com/

## Opcodes:

- OP_NC_INIT: initialize Notcurses; returns 1 on success, 0 on failure
- OP_NC_SHUTDOWN: shutdown; returns 0
- OP_NC_CLEAR: clear screen/plane; returns 0
- OP_NC_DRAW_TEXT: pops text, x, y; draws; returns 0
- OP_NC_GETCH: pops timeout_ms; returns codepoint or -1 on timeout/error
- OP_NC_GET_SIZE: push [rows, cols] of the stdplane, or -1 if unavailable
- OP_NC_SET_STYLE: pops style, bg_rgb, fg_rgb; sets colors/styles for stdplane; returns 0/-1
- OP_NC_DRAW_CHAR: pops ch, x, y; draws a single Unicode codepoint; returns 0/-1
- OP_NC_DRAW_HLINE: pops len, x, y, ch; draws a horizontal line; returns 0/-1
- OP_NC_DRAW_VLINE: pops len, x, y, ch; draws a vertical line; returns 0/-1
- OP_NC_BOX: pops x, y, w, h, style; draws a rectangle using ASCII/Unicode box glyphs; returns 0/-1
- OP_NC_FILL: pops x, y, w, h, ch; fills an area with a codepoint; returns 0/-1
- OP_NC_RENDER: forces a render; returns 0/-1

## Notes:

- Requires Notcurses development headers/libs.
- Behavior may vary across terminals; see the implementation for details.
- When Fun is built without Notcurses (FUN_WITH_NOTCURSES=OFF), these opcodes still exist but perform no-ops and return -1/0 as documented, allowing scripts to degrade gracefully.
