/* Workaround for linking Rust staticlib in no_std/abort mode on some toolchains */
void rust_eh_personality(void) {}
