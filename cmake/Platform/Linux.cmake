# Linux-specific configuration for Fun

# Default library directory (only set if not provided by user)
if(NOT DEFINED DEFAULT_LIB_DIR OR DEFAULT_LIB_DIR STREQUAL "")
  set(DEFAULT_LIB_DIR "/usr/share/fun/lib" CACHE PATH "Default library directory for Fun stdlib (override with -DDEFAULT_LIB_DIR=...)" FORCE)
endif()

# Handle musl toolchain selection when requested
if(FUN_USE_MUSL)
  find_program(_FUN_MUSL_CC NAMES musl-gcc musl-clang)
  if(_FUN_MUSL_CC)
    message(STATUS "FUN_USE_MUSL=ON: using musl toolchain: ${_FUN_MUSL_CC}")
    set(CMAKE_C_COMPILER "${_FUN_MUSL_CC}" CACHE FILEPATH "C compiler" FORCE)
    set(FUN_LIBC "musl" CACHE STRING "Selected C library")
    add_compile_definitions(FUN_LIBC_MUSL)
  else()
    message(WARNING "FUN_USE_MUSL=ON but no musl toolchain (musl-gcc or musl-clang) found. Falling back to default compiler (likely glibc).")
    set(FUN_LIBC "glibc" CACHE STRING "Selected C library")
    add_compile_definitions(FUN_LIBC_GLIBC)
  endif()
else()
  set(FUN_LIBC "glibc" CACHE STRING "Selected C library")
  add_compile_definitions(FUN_LIBC_GLIBC)
endif()
