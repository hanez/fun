/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-28
 */

#ifdef __unix__
#include <termios.h>
#endif

case OP_SERIAL_CONFIG: {
    /* Pops flow_control (int), stop_bits (int), parity (int), data_bits (int), fd (int); returns 1/0 */
    Value flowv = pop_value(vm);
    Value stopv = pop_value(vm);
    Value parityv = pop_value(vm);
    Value datav = pop_value(vm);
    Value fdv = pop_value(vm);
    int ok = 0;
#ifdef __unix__
    if (flowv.type != VAL_INT || stopv.type != VAL_INT || parityv.type != VAL_INT ||
        datav.type != VAL_INT || fdv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: serial_config expects (int fd, int data_bits, int parity, int stop_bits, int flow_control)\n");
    } else {
        int fd = (int)fdv.i;
        int data_bits = (int)datav.i;
        int parity = (int)parityv.i;
        int stop_bits = (int)stopv.i;
        int flow = (int)flowv.i;

        struct termios options;
        if (tcgetattr(fd, &options) == 0) {
            // Data bits
            options.c_cflag &= ~CSIZE;
            switch (data_bits) {
                case 5: options.c_cflag |= CS5; break;
                case 6: options.c_cflag |= CS6; break;
                case 7: options.c_cflag |= CS7; break;
                case 8: default: options.c_cflag |= CS8; break;
            }

            // Parity
            switch (parity) {
                case 0: // None
                    options.c_cflag &= ~PARENB;
                    break;
                case 1: // Odd
                    options.c_cflag |= PARENB;
                    options.c_cflag |= PARODD;
                    break;
                case 2: // Even
                    options.c_cflag |= PARENB;
                    options.c_cflag &= ~PARODD;
                    break;
            }

            // Stop bits
            if (stop_bits == 2) {
                options.c_cflag |= CSTOPB;
            } else {
                options.c_cflag &= ~CSTOPB;
            }

            // Flow control
#ifdef CRTSCTS
            if (flow == 1) { // Hardware (RTS/CTS)
                options.c_cflag |= CRTSCTS;
            } else {
                options.c_cflag &= ~CRTSCTS;
            }
#endif

            if (tcsetattr(fd, TCSANOW, &options) == 0) {
                ok = 1;
            }
        }
    }
#endif
    free_value(flowv);
    free_value(stopv);
    free_value(parityv);
    free_value(datav);
    free_value(fdv);
    push_value(vm, make_int(ok));
    break;
}
