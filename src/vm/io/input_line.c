/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
/**
 * @file input_line.c
 * @brief Implements the OP_INPUT_LINE opcode for interactive console input.
 *
 * This snippet handles the OP_INPUT_LINE instruction in the VM dispatch. It can
 * optionally print a prompt (taken from the stack) and can read input in a
 * hidden mode (terminal echo disabled) suitable for passwords.
 *
 * Operand bits (inst.operand):
 * - bit0 (1): Has prompt. When set, the top of the stack is popped and
 *             converted to string, printed without a trailing newline.
 * - bit1 (2): Hidden input. When set, terminal echo is temporarily disabled
 *             while reading the line (best-effort, platform dependent).
 *
 * Stack effects:
 * - If bit0 is set: pop(prompt)
 * - Always: push(result_string)
 *
 * Behavior:
 * - Converts an optional prompt Value to string using value_to_string_alloc,
 *   prints it to stdout without a newline, and flushes the stream.
 * - If hidden is requested, disables terminal echo (POSIX termios or Win32
 *   console modes) before reading.
 * - Reads a single line from stdin, accepting both "\n" and "\r\n" endings.
 * - Restores terminal echo if it was disabled and, when a prompt was printed,
 *   emits a newline for a better UX.
 * - Pushes the captured line as a Fun string (never NULL; empty string on
 *   failure or EOF).
 *
 * Errors and corner cases:
 * - Memory allocation failures are reported to stderr; an empty string is
 *   pushed in such cases to keep execution flowing.
 * - If echo toggling fails, input still proceeds with echo enabled.
 * - EOF before any character yields an empty string.
 *
 * Example:
 *   // Bytecode: [optional PUSH prompt], OP_INPUT_LINE(operand)
 *   // operand bit0=1 (has prompt), bit1=2 (hidden) can be combined
 *   // Stack before (bit0=1): ["Enter password: "]
 *   // Stack after: ["user-typed-line"]
 */

case OP_INPUT_LINE: {
  /* operand bit flags:
   *  bit0 (1): has prompt (string or any value convertible to string) — top of stack holds prompt when set
   *  bit1 (2): hidden input (do not echo typed characters)
   */
  int has_prompt = (inst.operand & 1) ? 1 : 0;
  int hidden = (inst.operand & 2) ? 1 : 0;
  if (has_prompt) {
    /* pop prompt value and print without newline */
    Value pv = pop_value(vm);
    char *pstr = value_to_string_alloc(&pv);
    if (pstr) {
      fputs(pstr, stdout);
      fflush(stdout);
      free(pstr);
    }
    free_value(pv);
  }

  /* For hidden input, temporarily disable terminal echo if possible */
  int echo_disabled = 0;
#ifdef _WIN32
  if (hidden) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin != INVALID_HANDLE_VALUE) {
      DWORD mode;
      if (GetConsoleMode(hStdin, &mode)) {
        DWORD newMode = mode & ~(ENABLE_ECHO_INPUT);
        if (SetConsoleMode(hStdin, newMode)) {
          echo_disabled = 1;
        }
      }
    }
  }
#else
  if (hidden) {
    /* POSIX termios */
    struct termios oldt;
    if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
      struct termios newt = oldt;
      newt.c_lflag &= ~(ECHO);
      if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == 0) {
        echo_disabled = 1;
      }
    }
  }
#endif

  /* read a line from stdin, dynamically grow buffer */
  size_t cap = 128;
  size_t len = 0;
  char *buf = (char *)malloc(cap);
  if (!buf) {
    fprintf(stderr, "Runtime error: out of memory reading input");
    push_value(vm, make_string(""));
    /* On early exit, try to restore echo if we turned it off */
    goto restore_echo_and_break;
  }

  int ch;
  while ((ch = fgetc(stdin)) != EOF) {
    if (ch == '\r') {
      /* Handle CRLF by consuming optional following '\n' */
      int next = fgetc(stdin);
      if (next != EOF && next != '\n') {
        ungetc(next, stdin);
      }
      break;
    }
    if (ch == '\n') {
      break;
    }
    if (len + 1 >= cap) {
      cap *= 2;
      char *nb = (char *)realloc(buf, cap);
      if (!nb) {
        free(buf);
        fprintf(stderr, "Runtime error: out of memory reading input");
        push_value(vm, make_string(""));
        goto push_done;
      }
      buf = nb;
    }
    buf[len++] = (char)ch;
  }

  /* null-terminate */
  if (len + 1 >= cap) {
    char *nb = (char *)realloc(buf, len + 1);
    if (!nb) {
      free(buf);
      fprintf(stderr, "Runtime error: out of memory finalizing input");
      push_value(vm, make_string(""));
      goto push_done;
    }
    buf = nb;
  }
  buf[len] = '\0';

  /* push as Fun string */
  push_value(vm, make_string(buf));
  free(buf);

push_done:
  /* If we disabled echo, restore terminal settings and print a newline for UX */
#ifdef _WIN32
  if (echo_disabled) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin != INVALID_HANDLE_VALUE) {
      DWORD mode;
      if (GetConsoleMode(hStdin, &mode)) {
        /* Re-enable ECHO flag */
        mode |= ENABLE_ECHO_INPUT;
        SetConsoleMode(hStdin, mode);
      }
    }
    if (has_prompt) {
      fputc('\n', stdout);
      fflush(stdout);
    }
  }
#else
  if (echo_disabled) {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) == 0) {
      t.c_lflag |= ECHO;
      tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
    if (has_prompt) {
      fputc('\n', stdout);
      fflush(stdout);
    }
  }
#endif
restore_echo_and_break:
  break;
}
