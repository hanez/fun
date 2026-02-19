# Tcl/Tk (GUI) extension (optional)
- CMake option: FUN_WITH_TCLTK=ON
- Purpose: Basic GUI functionality via Tcl/Tk.
- Homepage: https://www.tcl.tk/
Opcodes:
- OP_TK_EVAL: pops script string; pushes rc (0=OK)
- OP_TK_RESULT: pushes last Tcl result string
- OP_TK_LOOP: enters main event loop; pushes Nil when done
- OP_TK_WM_TITLE: pops title string; sets window title; pushes rc
- OP_TK_LABEL: pops text, id; creates/updates label .id; pushes rc
- OP_TK_BUTTON: pops text, id; creates/updates button .id; pushes rc
- OP_TK_PACK: pops id; packs .id; pushes rc
- OP_TK_BIND: pops command, event, id; binds; pushes rc
Notes:
- Requires Tcl/Tk development headers/libs.
- GUI behavior depends on your desktop environment/window manager.
