/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-11 (2025-12-11 migrated from src/vm.c)
 */

#ifdef FUN_WITH_TCLTK
#include <tcl.h>
#include <tk.h>
static Tcl_Interp* g_fun_tcl_interp = NULL;

static void fun_tk_init_once(void) {
    if (g_fun_tcl_interp) return;
    Tcl_FindExecutable(NULL);
    g_fun_tcl_interp = Tcl_CreateInterp();
    if (!g_fun_tcl_interp) return;
    if (Tcl_Init(g_fun_tcl_interp) != TCL_OK) {
        fprintf(stderr, "Tcl_Init failed: %s\n", Tcl_GetStringResult(g_fun_tcl_interp));
    }
    if (Tk_Init(g_fun_tcl_interp) != TCL_OK) {
        fprintf(stderr, "Tk_Init failed: %s\n", Tcl_GetStringResult(g_fun_tcl_interp));
    }
    /* Ensure the app terminates if the main window is closed via window manager */
    /* Best-effort: set WM_DELETE_WINDOW handler to exit the process. */
    Tcl_Eval(g_fun_tcl_interp, "wm protocol . WM_DELETE_WINDOW {exit 0}");
}

static int fun_tk_eval_script(const char *script) {
    fun_tk_init_once();
    if (!g_fun_tcl_interp) return -1;
    int rc = Tcl_Eval(g_fun_tcl_interp, script ? script : "");
    return rc; /* TCL_OK = 0 */
}

static const char* fun_tk_get_result(void) {
    fun_tk_init_once();
    if (!g_fun_tcl_interp) return "";
    return Tcl_GetStringResult(g_fun_tcl_interp);
}

static void fun_tk_loop(void) {
    fun_tk_init_once();
    if (!g_fun_tcl_interp) return;
    /* Drive Tk event loop until all main windows are closed */
    while (Tk_GetNumMainWindows() > 0) {
        while (Tcl_DoOneEvent(0)) {}
        /* tiny sleep to avoid busy spin */
#ifdef _WIN32
#include <windows.h>
        Sleep(1);
#else
#include <time.h>
        struct timespec ts = {0, 1000000}; /* 1 ms */
        nanosleep(&ts, NULL);
#endif
    }
}
#else
/* Stubs when Tcl/Tk is disabled */
static void fun_tk_init_once(void) { (void)0; }
static int fun_tk_eval_script(const char *script) { (void)script; return -1; }
static const char* fun_tk_get_result(void) { return ""; }
static void fun_tk_loop(void) { (void)0; }
#endif
