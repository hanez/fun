## LibreSSL optional integration (MD5, etc.)

option(FUN_WITH_LIBRESSL "Enable LibreSSL-based features (e.g., md5)" OFF)

set(LIBRESSL_INCLUDE_DIRS "")
set(LIBRESSL_LINK_LIBS "")

if(FUN_WITH_LIBRESSL)
  # We must NOT depend on OpenSSL being installed. Detect LibreSSL directly.
  # Strategy (in order):
  #  1) Prefer pkg-config 'libressl' if available (brings ssl+crypto and headers)
  #  2) Else, prefer explicit LibreSSL include prefix (/usr/include/libressl)
  #  3) Else, fall back to generic OpenSSL-compatible headers, but only if they
  #     are provided by LibreSSL (detected heuristically via opensslv.h content)
  #  4) Link against libcrypto (from LibreSSL). No OpenSSL::Crypto usage.

  include(CheckIncludeFile)
  include(FindPkgConfig)

  set(_libressl_found FALSE)

  # 1) Try pkg-config: libressl (meta) or libtls (often implies LibreSSL presence)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBRESSL_PKG QUIET libressl)
    if(LIBRESSL_PKG_FOUND)
      list(APPEND LIBRESSL_INCLUDE_DIRS ${LIBRESSL_PKG_INCLUDE_DIRS})
      list(APPEND LIBRESSL_LINK_LIBS ${LIBRESSL_PKG_LIBRARIES})
      set(_libressl_found TRUE)
    else()
      # Some distros provide only crypto/ssl pc files from LibreSSL
      pkg_check_modules(LIBRESSL_CRYPTO QUIET libcrypto)
      pkg_check_modules(LIBRESSL_SSL QUIET libssl)
      if(LIBRESSL_CRYPTO_FOUND)
        list(APPEND LIBRESSL_INCLUDE_DIRS ${LIBRESSL_CRYPTO_INCLUDE_DIRS})
        list(APPEND LIBRESSL_LINK_LIBS ${LIBRESSL_CRYPTO_LIBRARIES})
        if(LIBRESSL_SSL_FOUND)
          list(APPEND LIBRESSL_INCLUDE_DIRS ${LIBRESSL_SSL_INCLUDE_DIRS})
          list(APPEND LIBRESSL_LINK_LIBS ${LIBRESSL_SSL_LIBRARIES})
        endif()
        set(_libressl_found TRUE)
      endif()
    endif()
  endif()

  # 2) Prefer explicit LibreSSL include prefix if present
  if(EXISTS "/usr/include/libressl")
    list(PREPEND LIBRESSL_INCLUDE_DIRS "/usr/include/libressl")
    set(_libressl_found TRUE)
  endif()

  # 3) Heuristic: check if the generic openssl headers are from LibreSSL
  if(NOT _libressl_found)
    # Try to find opensslv.h and see if it defines LIBRESSL_VERSION_NUMBER
    find_path(_GEN_OPENSSL_INCLUDE_DIR openssl/opensslv.h
      PATHS /usr/include /usr/local/include)
    if(_GEN_OPENSSL_INCLUDE_DIR)
      file(READ "${_GEN_OPENSSL_INCLUDE_DIR}/openssl/opensslv.h" _opensslv_h CONTENT_STRIP_TRAILING_WHITESPACE)
      string(FIND "${_opensslv_h}" "LIBRESSL_VERSION_NUMBER" _idx)
      if(NOT _idx EQUAL -1)
        list(APPEND LIBRESSL_INCLUDE_DIRS "${_GEN_OPENSSL_INCLUDE_DIR}")
        set(_libressl_found TRUE)
      endif()
    endif()
  endif()

  # 4) Find the crypto library (from LibreSSL). Avoid CMake's OpenSSL package and target.
  if(NOT LIBRESSL_LINK_LIBS)
    # Try to locate libcrypto first
    find_library(LIBRESSL_CRYPTO_LIB NAMES crypto libcrypto PATHS
      /usr/lib /usr/local/lib /lib)
    if(LIBRESSL_CRYPTO_LIB)
      list(APPEND LIBRESSL_LINK_LIBS ${LIBRESSL_CRYPTO_LIB})
    endif()
  endif()

  if(NOT _libressl_found OR NOT LIBRESSL_LINK_LIBS)
    message(FATAL_ERROR "FUN_WITH_LIBRESSL=ON but LibreSSL headers and/or libcrypto not found.\n"
                         "Tried pkg-config (libressl/libcrypto), /usr/include/libressl, and generic openssl headers from LibreSSL.")
  endif()

  # Ensure the compile definition is visible even if targets are created later
  add_compile_definitions(FUN_WITH_LIBRESSL=1)

  # Propagate to main targets if they exist in this scope
  if(TARGET fun_core)
    target_link_libraries(fun_core PRIVATE ${LIBRESSL_LINK_LIBS})
    target_include_directories(fun_core PRIVATE ${LIBRESSL_INCLUDE_DIRS})
    target_compile_definitions(fun_core PRIVATE FUN_WITH_LIBRESSL=1)
  endif()
  if(TARGET fun)
    target_link_libraries(fun PRIVATE ${LIBRESSL_LINK_LIBS})
    target_include_directories(fun PRIVATE ${LIBRESSL_INCLUDE_DIRS})
    target_compile_definitions(fun PRIVATE FUN_WITH_LIBRESSL=1)
  endif()
  if(TARGET fun_test)
    target_link_libraries(fun_test PRIVATE ${LIBRESSL_LINK_LIBS})
    target_include_directories(fun_test PRIVATE ${LIBRESSL_INCLUDE_DIRS})
    target_compile_definitions(fun_test PRIVATE FUN_WITH_LIBRESSL=1)
  endif()
endif()
