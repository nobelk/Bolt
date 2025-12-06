# CUDA-specific configuration

if(BUILD_CUDA)
    # Set CUDA architectures based on target GPUs
    # 70 = Volta (V100)
    # 75 = Turing (RTX 20xx)
    # 80 = Ampere (A100, RTX 30xx)
    # 86 = Ampere (RTX 3090)
    # 89 = Ada Lovelace (RTX 40xx)
    # 90 = Hopper (H100)

    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES "70;75;80;86;89;90" CACHE STRING
            "CUDA architectures to compile for")
    endif()

    # CUDA compiler flags
    set(CUDA_NVCC_FLAGS
        --expt-relaxed-constexpr
        --extended-lambda
        -use_fast_math
    )

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        list(APPEND CUDA_NVCC_FLAGS -G -g)  # Enable GPU debugging
    else()
        list(APPEND CUDA_NVCC_FLAGS -O3 -lineinfo)
    endif()

    # Apply CUDA flags to targets
    function(target_cuda_compile_options target_name)
        target_compile_options(${target_name} PRIVATE
            $<$<COMPILE_LANGUAGE:CUDA>:${CUDA_NVCC_FLAGS}>
        )
    endfunction()
endif()
