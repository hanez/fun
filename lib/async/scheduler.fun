/*
 * Cooperative asyncio helpers (library-level) for Fun
 *
 * This module provides a tiny, user-space scheduler built on top of the
 * existing non-blocking FD helpers (fd_set_nonblock, fd_poll_read, fd_poll_write).
 *
 * There is no VM-level suspension: tasks must be written as small step
 * functions that advance a state machine a little and then return so the
 * scheduler can run other tasks. Use await_read/await_write to probe IO
 * readiness with short timeouts and yield() to voluntarily give up control.
 *
 * API (minimal):
 * - task_spawn(step_fn, state_map) -> task_handle (a Map)
 * - run_until_done() -> runs all spawned tasks until every task has state.done == 1
 * - run_once() -> performs one scheduling tick over all tasks
 * - await_read(fd, timeout_ms) -> 1 if readable else 0; -1 on error
 * - await_write(fd, timeout_ms) -> 1 if writable else 0; -1 on error
 * - yield() -> 1 (hint to return to scheduler)
 * - async_sleep_mark(state, ms) -> marks state to sleep for ms; scheduler will skip until wake
 */

__tasks = []

fun __now_ms()
  return time_now_ms()

/* Spawn a cooperative task.
 * step_fn: function taking a single Map parameter (the task object itself)
 * state_map: optional Map to seed task fields; may contain 'done' flag initially 0
 */
fun task_spawn(step_fn, state_map)
  t = {}
  t.fn = step_fn
  if (typeof(state_map) == "Map")
    /* shallow copy of provided state (no "for in" syntax in Fun parser) */
    _ks = keys(state_map)
    _i = 0
    _n = len(_ks)
    while (_i < _n)
      _k = _ks[_i]
      t[_k] = state_map[_k]
      _i = _i + 1
  /* normalize done flag to 0/1 */
  _d = to_number(t.done)
  if (_d == 0 || _d == 1)
    t.done = _d
  else
    t.done = 0
  t._sleep_until = 0
  push(__tasks, t)
  return t

/* Mark the task state to sleep (skip execution) for ms milliseconds */
fun async_sleep_mark(state, ms)
  state._sleep_until = __now_ms() + to_number(ms)
  return 1

/* Voluntary cooperative yield helper (avoid reserved keyword name) */
fun co_yield()
  return 1

/* Probe for readability. Returns 1 if readable, 0 on timeout or EOF, -1 on error. */
fun await_read(fd, timeout_ms)
  return fd_poll_read(to_number(fd), to_number(timeout_ms))

/* Probe for writability. Returns 1 if writable, 0 on timeout, -1 on error. */
fun await_write(fd, timeout_ms)
  return fd_poll_write(to_number(fd), to_number(timeout_ms))

/* Execute one scheduling tick: iterate over all tasks and invoke their step
 * function if not done and if not sleeping. Removes finished tasks at the end.
 */
fun run_once()
  i = 0
  n = len(__tasks)
  now = __now_ms()
  while (i < n)
    t = __tasks[i]
    if (typeof(t) != "Map")
      i = i + 1
      continue
    if (to_number(t.done) != 1)
      if (to_number(t._sleep_until) > now)
        i = i + 1
        continue
      else
        t._sleep_until = 0
        /* Call step function if present */
        if (t.fn != nil)
          t.fn(t)
    i = i + 1

  /* Rebuild task list keeping only unfinished tasks (single pass) */
  tmp = []
  j = 0
  m = len(__tasks)
  while (j < m)
    tt = __tasks[j]
    if (typeof(tt) != "Map" || to_number(tt.done) != 1)
      push(tmp, tt)
    j = j + 1
  __tasks = tmp
  return 1

/* Run until all tasks report done == 1. To avoid busy spinning, sleep 1ms per tick. */
fun run_until_done()
  while (len(__tasks) > 0)
    run_once()
    /* Tiny pause to reduce CPU when nothing is ready */
    sleep_ms(1)
  return 1
