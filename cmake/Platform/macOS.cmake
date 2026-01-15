# macOS-specific configuration for Fun

# Default library directory (only set if not provided by user)
if(NOT DEFINED DEFAULT_LIB_DIR OR DEFAULT_LIB_DIR STREQUAL "")
  set(DEFAULT_LIB_DIR "/Library/Application Support/fun/lib" CACHE PATH "Default library directory for Fun stdlib (override with -DDEFAULT_LIB_DIR=...)" FORCE)
endif()

# libc macro (non-musl path)
set(FUN_LIBC "glibc" CACHE STRING "Selected C library")
add_compile_definitions(FUN_LIBC_GLIBC)
