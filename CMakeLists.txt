cmake_minimum_required(VERSION 3.25)

project(Bolt
    VERSION 0.1.0
    DESCRIPTION "Real-Time GPU-Accelerated Constraint Satisfaction Engine"
    LANGUAGES CXX
)

# ============================================================================
# Project Options
# ============================================================================
option(BUILD_CUDA "Enable CUDA acceleration" OFF)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_SANITIZERS "Enable sanitizers (Debug builds)" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable clang-tidy" OFF)

# ============================================================================
# C++ Standard and Features
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# vcpkg Integration
# ============================================================================
# vcpkg should be integrated via CMAKE_TOOLCHAIN_FILE
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

# ============================================================================
# CUDA Configuration (Optional)
# ============================================================================
if(BUILD_CUDA)
    enable_language(CUDA)
    find_package(CUDAToolkit 12.0 REQUIRED)

    # CUDA architectures (compute capability 7.0+)
    set(CMAKE_CUDA_ARCHITECTURES "70;75;80;86;89;90" CACHE STRING "CUDA architectures")
    set(CMAKE_CUDA_STANDARD 20)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    message(STATUS "CUDA enabled - Version: ${CUDAToolkit_VERSION}")
    message(STATUS "CUDA Architectures: ${CMAKE_CUDA_ARCHITECTURES}")
endif()

# ============================================================================
# Dependencies (via vcpkg)
# ============================================================================
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if(BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
    enable_testing()
endif()

# ============================================================================
# CMake Modules
# ============================================================================
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(ENABLE_WARNINGS)
    include(CompilerWarnings)
endif()

if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES Debug)
    include(Sanitizers)
endif()

if(ENABLE_STATIC_ANALYSIS)
    include(StaticAnalyzers)
endif()

# ============================================================================
# Subdirectories
# ============================================================================
add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================================
# Installation Rules
# ============================================================================
include(GNUInstallDirs)

install(DIRECTORY include/bolt
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(TARGETS bolt_core
    EXPORT BoltTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT BoltTargets
    FILE BoltTargets.cmake
    NAMESPACE Bolt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bolt
)

# ============================================================================
# Package Configuration
# ============================================================================
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/BoltConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/BoltConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bolt
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/BoltConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${PROJECT_BINARY_DIR}/BoltConfig.cmake"
    "${PROJECT_BINARY_DIR}/BoltConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bolt
)

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "Bolt Configuration Summary:")
message(STATUS "  Version:              ${PROJECT_VERSION}")
message(STATUS "  C++ Standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "  CUDA Support:         ${BUILD_CUDA}")
message(STATUS "  Build Tests:          ${BUILD_TESTS}")
message(STATUS "  Build Examples:       ${BUILD_EXAMPLES}")
message(STATUS "  Compiler Warnings:    ${ENABLE_WARNINGS}")
message(STATUS "  Sanitizers:           ${ENABLE_SANITIZERS}")
message(STATUS "  Static Analysis:      ${ENABLE_STATIC_ANALYSIS}")
message(STATUS "")
