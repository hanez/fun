/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-30
 */

/* Cross-platform minimal threading support embedded into vm.c TU */

#include <stdint.h>
#include <time.h>

#ifdef _WIN32
  #include <windows.h>
  typedef HANDLE fun_thread_handle_t;
  typedef DWORD fun_thread_ret_t;
  #define FUN_THREAD_CALL WINAPI
  #define fun_sleep_ms(ms) Sleep((DWORD)(ms))
#else
  #include <pthread.h>
  #include <unistd.h>
  typedef pthread_t fun_thread_handle_t;
  typedef void* fun_thread_ret_t;
  #define FUN_THREAD_CALL
  static inline void fun_sleep_ms(long ms) {
      if (ms <= 0) return;
      struct timespec ts;
      ts.tv_sec = ms / 1000;
      ts.tv_nsec = (ms % 1000) * 1000000L;
      nanosleep(&ts, NULL);
  }
#endif

#ifndef FUN_MAX_THREADS
#define FUN_MAX_THREADS 64
#endif

typedef struct {
    fun_thread_handle_t handle;
    int used;
    int done;
    Value result; /* owned */
#ifdef _WIN32
    DWORD threadId;
#endif
} FunThreadEntry;

static FunThreadEntry g_threads[FUN_MAX_THREADS];

#ifdef _WIN32
static CRITICAL_SECTION g_thr_lock;
static int g_thr_lock_inited = 0;
static void fun_thr_lock_init(void) {
    if (!g_thr_lock_inited) {
        InitializeCriticalSection(&g_thr_lock);
        g_thr_lock_inited = 1;
    }
}
static void fun_lock(void)   { if (!g_thr_lock_inited) fun_thr_lock_init(); EnterCriticalSection(&g_thr_lock); }
static void fun_unlock(void) { LeaveCriticalSection(&g_thr_lock); }
#else
static pthread_mutex_t g_thr_lock = PTHREAD_MUTEX_INITIALIZER;
static void fun_lock(void)   { pthread_mutex_lock(&g_thr_lock); }
static void fun_unlock(void) { pthread_mutex_unlock(&g_thr_lock); }
#endif

typedef struct {
    Bytecode *fn;   /* function to call */
    int argc;
    Value *args;    /* array of argc Values (owned by task, will be freed) */
    int slot;       /* registry slot */
} FunTask;

#ifdef _WIN32
static fun_thread_ret_t FUN_THREAD_CALL fun_thread_main(LPVOID param)
#else
static fun_thread_ret_t fun_thread_main(void *param)
#endif
{
    FunTask *task = (FunTask*)param;
    VM tvm;
    vm_init(&tvm);

    /* Build wrapper: LOAD_CONST <fn>; LOAD_CONST <arg0> ...; CALL argc; HALT */
    Bytecode *wrap = bytecode_new();
    int cFn = bytecode_add_constant(wrap, copy_value(&(Value){ .type = VAL_FUNCTION, .fn = task->fn }));
    bytecode_add_instruction(wrap, OP_LOAD_CONST, cFn);
    for (int i = 0; i < task->argc; ++i) {
        int cArg = bytecode_add_constant(wrap, deep_copy_value(&task->args[i]));
        bytecode_add_instruction(wrap, OP_LOAD_CONST, cArg);
    }
    bytecode_add_instruction(wrap, OP_CALL, task->argc);
    bytecode_add_instruction(wrap, OP_HALT, 0);

    vm_run(&tvm, wrap);

    /* Take the top of stack as result if present, else Nil */
    Value res = make_nil();
    if (tvm.sp >= 0) {
        res = deep_copy_value(&tvm.stack[tvm.sp]);
    }

    /* cleanup */
    for (int i = 0; i < task->argc; ++i) {
        free_value(task->args[i]);
    }
    free(task->args);
    bytecode_free(wrap);

    /* store result */
    fun_lock();
    g_threads[task->slot].result = res;
    g_threads[task->slot].done = 1;
    fun_unlock();

    free(task);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

static int fun_alloc_thread_slot(void) {
    fun_lock();
    int idx = -1;
    for (int i = 0; i < FUN_MAX_THREADS; ++i) {
        if (!g_threads[i].used) { g_threads[i].used = 1; g_threads[i].done = 0; g_threads[i].result = make_nil(); idx = i; break; }
    }
    fun_unlock();
    return idx;
}

static int fun_thread_spawn(Value fnVal, Value argsMaybe, int hasArgs) {
    if (fnVal.type != VAL_FUNCTION || !fnVal.fn) {
        fprintf(stderr, "Runtime error: thread_spawn expects Function as first argument\n");
        return 0;
    }

    /* Collect args */
    int argc = 0;
    Value *args = NULL;

    if (hasArgs) {
        if (argsMaybe.type == VAL_ARRAY && argsMaybe.arr) {
            int n = array_length(&argsMaybe);
            if (n > 0) {
                args = (Value*)calloc((size_t)n, sizeof(Value));
                if (!args) n = 0;
                for (int i = 0; i < n; ++i) {
                    Value vi;
                    if (array_get_copy(&argsMaybe, i, &vi)) {
                        args[i] = deep_copy_value(&vi);
                        free_value(vi);
                    } else {
                        args[i] = make_nil();
                    }
                }
                argc = n;
            }
        } else if (argsMaybe.type != VAL_NIL) {
            args = (Value*)calloc(1, sizeof(Value));
            if (args) {
                args[0] = deep_copy_value(&argsMaybe);
                argc = 1;
            }
        }
    }

    int slot = fun_alloc_thread_slot();
    if (slot < 0) {
        fprintf(stderr, "Runtime error: too many threads\n");
        /* free args */
        for (int i = 0; i < argc; ++i) free_value(args[i]);
        free(args);
        return 0;
    }

    FunTask *task = (FunTask*)calloc(1, sizeof(FunTask));
    if (!task) {
        for (int i = 0; i < argc; ++i) free_value(args[i]);
        free(args);
        fun_lock(); g_threads[slot].used = 0; fun_unlock();
        return 0;
    }
    task->fn = fnVal.fn;
    task->argc = argc;
    task->args = args;
    task->slot = slot;

#ifdef _WIN32
    HANDLE h = CreateThread(NULL, 0, fun_thread_main, (LPVOID)task, 0, &g_threads[slot].threadId);
    if (!h) {
        fprintf(stderr, "Runtime error: CreateThread failed\n");
        for (int i = 0; i < argc; ++i) free_value(args[i]);
        free(args);
        free(task);
        fun_lock(); g_threads[slot].used = 0; fun_unlock();
        return 0;
    }
    fun_lock(); g_threads[slot].handle = h; fun_unlock();
#else
    pthread_t tid;
    int rc = pthread_create(&tid, NULL, fun_thread_main, (void*)task);
    if (rc != 0) {
        fprintf(stderr, "Runtime error: pthread_create failed\n");
        for (int i = 0; i < argc; ++i) free_value(args[i]);
        free(args);
        free(task);
        fun_lock(); g_threads[slot].used = 0; fun_unlock();
        return 0;
    }
    fun_lock(); g_threads[slot].handle = tid; fun_unlock();
#endif

    return slot + 1; /* external thread id: 1..N */
}

static Value fun_thread_join(int tid) {
    if (tid <= 0 || tid > FUN_MAX_THREADS) {
        fprintf(stderr, "Runtime error: thread_join invalid id %d\n", tid);
        return make_nil();
    }
    int idx = tid - 1;

#ifdef _WIN32
    fun_lock();
    HANDLE h = g_threads[idx].handle;
    int used = g_threads[idx].used;
    fun_unlock();
    if (!used || !h) return make_nil();
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
#else
    fun_lock();
    pthread_t h = g_threads[idx].handle;
    int used = g_threads[idx].used;
    fun_unlock();
    if (!used) return make_nil();
    pthread_join(h, NULL);
#endif

    /* fetch result and free slot */
    fun_lock();
    Value res = deep_copy_value(&g_threads[idx].result);
    free_value(g_threads[idx].result);
    g_threads[idx].result = make_nil();
    g_threads[idx].used = 0;
    g_threads[idx].done = 0;
#ifdef _WIN32
    g_threads[idx].threadId = 0;
#endif
    fun_unlock();

    return res;
}
