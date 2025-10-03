#ifndef FUN_REPL_H
#define FUN_REPL_H

#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FUN_WITH_REPL
// Runs the interactive REPL using the provided, already-initialized VM.
// Returns 0 on normal exit.
int fun_run_repl(VM *vm);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FUN_REPL_H
