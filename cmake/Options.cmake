# Global options and defaults

# Convenience: default path to bundled stdlib
set(FUN_LIB "${CMAKE_SOURCE_DIR}/lib" CACHE PATH "Path to bundled Fun stdlib")

# Optional: build using musl libc instead of glibc on Linux (OFF by default)
option(FUN_USE_MUSL "Use musl libc toolchain when available (Linux only)" OFF)

# Platform-specific configuration (DEFAULT_LIB_DIR defaults, libc defines/toolchain tweaks)
if(WIN32)
  include(${CMAKE_SOURCE_DIR}/cmake/Platform/Windows.cmake)
elseif(APPLE)
  include(${CMAKE_SOURCE_DIR}/cmake/Platform/macOS.cmake)
elseif(UNIX)
  include(${CMAKE_SOURCE_DIR}/cmake/Platform/Linux.cmake)
endif()

# If user provided DEFAULT_LIB_DIR, keep it; otherwise it should be set by platform include above
set(DEFAULT_LIB_DIR "${DEFAULT_LIB_DIR}" CACHE PATH "Default library directory for Fun stdlib (override with -DDEFAULT_LIB_DIR=...)" FORCE)

# Ensure trailing slash for DEFAULT_LIB_DIR if it is defined
if(DEFINED DEFAULT_LIB_DIR AND NOT DEFAULT_LIB_DIR STREQUAL "")
  if(NOT DEFAULT_LIB_DIR MATCHES "/$")
    set(DEFAULT_LIB_DIR "${DEFAULT_LIB_DIR}/")
  endif()
endif()

# Static linking is deprecated/unsupported: always build dynamically
option(FUN_LINK_STATIC "(Deprecated) Attempt to link statically — ignored; dynamic linking is enforced" OFF)
if(FUN_LINK_STATIC)
  message(WARNING "FUN_LINK_STATIC is deprecated and ignored. Building with dynamic linking.")
endif()

# Debug option to enable verbose parser/VM logging
option(FUN_DEBUG "Enable extra debug logging in Fun" OFF)
