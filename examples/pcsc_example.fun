#!/usr/bin/env fun

// Minimal PCSC example for Fun language
// Establish context and list readers (prints [] if none or unsupported)

class Pcsc()
  fun list_readers(this)
    ctx = pcsc_establish()
    readers = pcsc_list_readers(ctx)
    print(readers)
    _ = pcsc_release(ctx)

p = Pcsc()
p.list_readers()
