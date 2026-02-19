# Notcurses (TUI) extension (optional)

- CMake option: FUN_WITH_NOTCURSES=ON
- Purpose: Terminal UI capabilities via the Notcurses library.
- Homepage: https://notcurses.com/

Opcodes:
- OP_NC_INIT: initialize Notcurses; returns 1 on success, 0 on failure
- OP_NC_SHUTDOWN: shutdown; returns 0
- OP_NC_CLEAR: clear screen/plane; returns 0
- OP_NC_DRAW_TEXT: pops text, x, y; draws; returns 0
- OP_NC_GETCH: pops timeout_ms; returns codepoint or -1 on timeout/error

Notes:
- Requires Notcurses development headers/libs.
- Behavior may vary across terminals; see the implementation for details.
