---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - PC/SC (smart cards) extension (optional)
subtitle: Documentation for PC/SC (smart cards) extension (optional)
description: Documentation for PC/SC (smart cards) extension (optional)
permalink: /documentation/external/pcsc/
lang: en
tags:
- documentation
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
- docs
- specifications
- specs
- repl
---

# PC/SC (smart cards) extension (optional)

- CMake option: FUN_WITH_PCSC=ON
- Purpose: Access smart card readers/cards via PC/SC (pcsclite).
- Homepage: https://pcsclite.apdu.fr/

## Opcodes:

- OP_PCSC_ESTABLISH: returns context id (>0) or 0
- OP_PCSC_RELEASE: pops ctx id; returns 1/0
- OP_PCSC_LIST_READERS: pops ctx id; returns array of reader names
- OP_PCSC_CONNECT: pops reader, ctx id; returns handle (>0) or 0
- OP_PCSC_DISCONNECT: pops handle id; returns 1/0
- OP_PCSC_TRANSMIT: pops apdu, handle id; returns map with data/SW/rc

## Notes:

- Requires PC/SC lite development headers/libs.
- Behavior and availability depend on platform and reader drivers.
