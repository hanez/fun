/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

/* PCSC transmit */
case OP_PCSC_TRANSMIT: {
#ifdef FUN_WITH_PCSC
    /* pops apdu array, handle_id */
    Value vapdu = pop_value(vm);
    Value vh = pop_value(vm);
    int hid = (int)vh.i;
    free_value(vh);
    pcsc_card_entry *ce = pcsc_get_card(hid);

    Value m = make_map_empty();
    map_set(&m, "data", make_array_from_values(NULL, 0));
    map_set(&m, "sw1", make_int(-1));
    map_set(&m, "sw2", make_int(-1));
    map_set(&m, "code", make_int(-1));

    if (!ce || vapdu.type != VAL_ARRAY) {
        free_value(vapdu);
        push_value(vm, m);
        break;
    }
    int n = array_length(&vapdu);
    if (n < 0) n = 0;
    unsigned char sbuf[4096];
    if (n > (int)sizeof(sbuf)) n = (int)sizeof(sbuf);
    for (int i = 0; i < n; ++i) {
        Value tmp;
        if (array_get_copy(&vapdu, i, &tmp)) {
            int64_t v = tmp.type == VAL_INT ? tmp.i : 0;
            sbuf[i] = (unsigned char)(v & 0xFF);
            free_value(tmp);
        } else {
            sbuf[i] = 0;
        }
    }
    free_value(vapdu);

    SCARD_IO_REQUEST pio;
    if (ce->proto == SCARD_PROTOCOL_T0) pio = *SCARD_PCI_T0;
    else if (ce->proto == SCARD_PROTOCOL_T1) pio = *SCARD_PCI_T1;
    else { pio = *SCARD_PCI_T1; }

    unsigned char rbuf[4096];
    DWORD rlen = sizeof(rbuf);
    LONG rv = SCardTransmit(ce->h, &pio, sbuf, (DWORD)n, NULL, rbuf, &rlen);

    int sw1 = -1, sw2 = -1;
    int datalen = 0;
    if (rv == SCARD_S_SUCCESS && rlen >= 2) {
        sw1 = rbuf[rlen - 2];
        sw2 = rbuf[rlen - 1];
        datalen = (int)rlen - 2;
    }

    map_set(&m, "sw1", make_int(sw1));
    map_set(&m, "sw2", make_int(sw2));
    map_set(&m, "code", make_int((int)rv));

    if (datalen > 0) {
        Value *vals = (Value*)malloc(sizeof(Value) * (size_t)datalen);
        if (!vals) {
            map_set(&m, "data", make_array_from_values(NULL, 0));
        } else {
            for (int i = 0; i < datalen; ++i) vals[i] = make_int((int64_t)rbuf[i]);
            Value arr = make_array_from_values(vals, datalen);
            for (int i = 0; i < datalen; ++i) free_value(vals[i]);
            free(vals);
            map_set(&m, "data", arr);
        }
    }

    push_value(vm, m);
#else
    Value vapdu = pop_value(vm); free_value(vapdu);
    Value vh = pop_value(vm); free_value(vh);
    Value m = make_map_empty();
    map_set(&m, "data", make_array_from_values(NULL, 0));
    map_set(&m, "sw1", make_int(-1));
    map_set(&m, "sw2", make_int(-1));
    map_set(&m, "code", make_int(-2));
    push_value(vm, m);
#endif
    break;
}
