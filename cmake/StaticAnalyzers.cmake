# Enable static analysis tools

if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANGTIDY};
            -header-filter=${PROJECT_SOURCE_DIR};
            -warnings-as-errors=*;
        )
        message(STATUS "clang-tidy enabled")
    else()
        message(WARNING "clang-tidy requested but not found")
    endif()

    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        set(CMAKE_CXX_CPPCHECK
            ${CPPCHECK};
            --enable=all;
            --suppress=missingInclude;
            --inline-suppr;
            --std=c++20;
        )
        message(STATUS "cppcheck enabled")
    else()
        message(WARNING "cppcheck requested but not found")
    endif()
endif()
