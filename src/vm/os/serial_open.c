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
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/* Fallbacks for baud rates that might be missing on some systems */
#ifndef B50
#define B50 0000001
#endif
#ifndef B75
#define B75 0000002
#endif
#ifndef B110
#define B110 0000003
#endif
#ifndef B134
#define B134 0000004
#endif
#ifndef B150
#define B150 0000005
#endif
#ifndef B200
#define B200 0000006
#endif
#ifndef B300
#define B300 0000007
#endif
#ifndef B600
#define B600 0000010
#endif
#ifndef B1200
#define B1200 0000011
#endif
#ifndef B1800
#define B1800 0000012
#endif
#ifndef B2400
#define B2400 0000013
#endif
#ifndef B4800
#define B4800 0000014
#endif
#ifndef B9600
#define B9600 0000015
#endif
#ifndef B19200
#define B19200 0000016
#endif
#ifndef B38400
#define B38400 0000017
#endif
#ifndef B57600
#define B57600 0010001
#endif
#ifndef B115200
#define B115200 0010002
#endif
#ifndef B230400
#define B230400 0010003
#endif

#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif
#endif

case OP_SERIAL_OPEN: {
    /* Pops baud_rate (int), path (string); returns fd (int) or 0 */
    Value baudv = pop_value(vm);
    Value pathv = pop_value(vm);
    int fd = 0;
#ifdef __unix__
    if (baudv.type != VAL_INT || pathv.type != VAL_STRING) {
        fprintf(stderr, "Runtime type error: serial_open expects (string path, int baud_rate)\n");
        free_value(baudv);
        free_value(pathv);
        push_value(vm, make_int(0));
        break;
    }

    const char *path = pathv.s ? pathv.s : "";
    int baud = (int)baudv.i;
    speed_t speed;

    switch (baud) {
        case 50: speed = B50; break;
        case 75: speed = B75; break;
        case 110: speed = B110; break;
        case 134: speed = B134; break;
        case 150: speed = B150; break;
        case 200: speed = B200; break;
        case 300: speed = B300; break;
        case 600: speed = B600; break;
        case 1200: speed = B1200; break;
        case 1800: speed = B1800; break;
        case 2400: speed = B2400; break;
        case 4800: speed = B4800; break;
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        default: speed = B9600; break;
    }

    fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd != -1) {
        struct termios options;
        tcgetattr(fd, &options);
        cfsetispeed(&options, speed);
        cfsetospeed(&options, speed);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        options.c_oflag &= ~OPOST;
        tcsetattr(fd, TCSANOW, &options);
        fcntl(fd, F_SETFL, 0); // block on read
    } else {
        fd = 0;
    }
#endif
    free_value(baudv);
    free_value(pathv);
    push_value(vm, make_int(fd > 0 ? fd : 0));
    break;
}
