# ============================================================================
# Bolt Core Library
# ============================================================================

# Collect source files (placeholders - to be implemented)
set(BOLT_CORE_SOURCES
    # Core CSP Engine (will add .cpp files when implemented)
    # core/constraint.cpp
    # core/variable.cpp
    # core/domain.cpp
    # core/solver.cpp
    # core/propagation.cpp

    # Utilities (will add .cpp files when implemented)
    # utils/logger.cpp
    # utils/profiler.cpp
    # utils/config.cpp
)

set(BOLT_CORE_HEADERS
    # Internal headers (src/)
    core/constraint.hpp
    core/variable.hpp
    core/domain.hpp
    core/solver.hpp
    core/propagation.hpp
    utils/logger.hpp
    utils/profiler.hpp
    utils/config.hpp
)

set(BOLT_PUBLIC_HEADERS
    # Public API (include/bolt/)
    ${PROJECT_SOURCE_DIR}/include/bolt/bolt.hpp
    ${PROJECT_SOURCE_DIR}/include/bolt/types.hpp
    ${PROJECT_SOURCE_DIR}/include/bolt/constraints.hpp
    ${PROJECT_SOURCE_DIR}/include/bolt/export.hpp
)

# Create library target (INTERFACE for now since no .cpp files yet)
# Will change to STATIC once implementation files are added
add_library(bolt_core INTERFACE)

# Create alias for consistent naming
add_library(Bolt::Core ALIAS bolt_core)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(bolt_core
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# ============================================================================
# Link Dependencies
# ============================================================================
target_link_libraries(bolt_core
    INTERFACE
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# CUDA support (optional)
if(BUILD_CUDA)
    target_compile_definitions(bolt_core INTERFACE BOLT_CUDA_ENABLED)
    # CUDA sources would be added here in Phase 2
    # target_sources(bolt_core PRIVATE cuda/kernels.cu)
    # target_link_libraries(bolt_core PRIVATE CUDA::cudart)
endif()

# ============================================================================
# Compiler Settings
# ============================================================================
target_compile_features(bolt_core INTERFACE cxx_std_20)

# Note: When converting to STATIC library, move these settings to PRIVATE/PUBLIC
# Platform-specific settings
# if(MSVC)
#     target_compile_options(bolt_core PRIVATE /W4)
# else()
#     target_compile_options(bolt_core PRIVATE
#         -Wall -Wextra -Wpedantic
#         -Wno-unused-parameter  # Common in virtual functions
#     )
# endif()

# ============================================================================
# Properties
# ============================================================================
# set_target_properties(bolt_core PROPERTIES
#     VERSION ${PROJECT_VERSION}
#     SOVERSION ${PROJECT_VERSION_MAJOR}
#     OUTPUT_NAME "bolt"
#     POSITION_INDEPENDENT_CODE ON
# )
