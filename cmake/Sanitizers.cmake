# Enable sanitizers for debug builds

function(enable_sanitizers target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(SANITIZERS "")

        option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ON)
        if(ENABLE_SANITIZER_ADDRESS)
            list(APPEND SANITIZERS "address")
        endif()

        option(ENABLE_SANITIZER_UNDEFINED "Enable undefined behavior sanitizer" ON)
        if(ENABLE_SANITIZER_UNDEFINED)
            list(APPEND SANITIZERS "undefined")
        endif()

        option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
        if(ENABLE_SANITIZER_LEAK)
            list(APPEND SANITIZERS "leak")
        endif()

        option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
        if(ENABLE_SANITIZER_THREAD)
            if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
                message(WARNING "Thread sanitizer cannot be used with Address or Leak sanitizer")
            else()
                list(APPEND SANITIZERS "thread")
            endif()
        endif()

        list(JOIN SANITIZERS "," SANITIZER_LIST)

        if(SANITIZER_LIST)
            target_compile_options(${target_name} PRIVATE
                -fsanitize=${SANITIZER_LIST}
                -fno-omit-frame-pointer
            )
            target_link_options(${target_name} PRIVATE
                -fsanitize=${SANITIZER_LIST}
            )
            message(STATUS "Sanitizers enabled: ${SANITIZER_LIST}")
        endif()
    endif()
endfunction()
