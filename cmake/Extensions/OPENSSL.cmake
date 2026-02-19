## OpenSSL optional integration (MD5, etc.)

option(FUN_WITH_OPENSSL "Enable OpenSSL-based features (e.g., md5)" OFF)

set(OPENSSL_INCLUDE_DIRS "")
set(OPENSSL_LINK_LIBS "")

if(FUN_WITH_OPENSSL)
  find_package(OpenSSL REQUIRED)
  if(NOT OpenSSL_FOUND AND NOT OPENSSL_FOUND)
    message(FATAL_ERROR "FUN_WITH_OPENSSL=ON but OpenSSL not found")
  endif()

  # CMake 3.11+ standard variables
  if(TARGET OpenSSL::Crypto)
    set(OPENSSL_LINK_LIBS OpenSSL::Crypto)
  elseif(DEFINED OPENSSL_CRYPTO_LIBRARY)
    set(OPENSSL_LINK_LIBS ${OPENSSL_CRYPTO_LIBRARY})
  endif()

  if(DEFINED OPENSSL_INCLUDE_DIR)
    list(APPEND OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
  endif()

  # Propagate to main targets if they exist in this scope
  if(TARGET fun_core)
    target_link_libraries(fun_core PRIVATE ${OPENSSL_LINK_LIBS})
    target_include_directories(fun_core PRIVATE ${OPENSSL_INCLUDE_DIRS})
    target_compile_definitions(fun_core PRIVATE FUN_WITH_OPENSSL=1)
  endif()
  if(TARGET fun)
    target_link_libraries(fun PRIVATE ${OPENSSL_LINK_LIBS})
    target_include_directories(fun PRIVATE ${OPENSSL_INCLUDE_DIRS})
    target_compile_definitions(fun PRIVATE FUN_WITH_OPENSSL=1)
  endif()
  if(TARGET fun_test)
    target_link_libraries(fun_test PRIVATE ${OPENSSL_LINK_LIBS})
    target_include_directories(fun_test PRIVATE ${OPENSSL_INCLUDE_DIRS})
    target_compile_definitions(fun_test PRIVATE FUN_WITH_OPENSSL=1)
  endif()
endif()
