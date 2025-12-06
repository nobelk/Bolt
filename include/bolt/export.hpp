#pragma once

// DLL export/import macros for Windows
#ifdef _WIN32
    #ifdef BOLT_BUILD_SHARED
        #ifdef bolt_core_EXPORTS
            #define BOLT_API __declspec(dllexport)
        #else
            #define BOLT_API __declspec(dllimport)
        #endif
    #else
        #define BOLT_API
    #endif
#else
    #define BOLT_API
#endif

// CUDA availability macro
#ifdef BOLT_CUDA_ENABLED
    #define BOLT_CUDA_AVAILABLE 1
#else
    #define BOLT_CUDA_AVAILABLE 0
#endif
