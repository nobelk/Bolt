# ============================================================================
# Bolt Test Suite
# ============================================================================

include(GoogleTest)

# ============================================================================
# Unit Tests
# ============================================================================

set(UNIT_TEST_SOURCES
    # unit/test_main.cpp
    # unit/test_constraint.cpp
    # unit/test_variable.cpp
    # unit/test_domain.cpp
    # unit/test_solver.cpp
    # unit/test_propagation.cpp
)

# Uncomment when test files are created
# add_executable(bolt_unit_tests ${UNIT_TEST_SOURCES})
#
# target_link_libraries(bolt_unit_tests
#     PRIVATE
#         Bolt::Core
#         GTest::gtest
#         GTest::gtest_main
# )
#
# target_include_directories(bolt_unit_tests
#     PRIVATE
#         ${PROJECT_SOURCE_DIR}/src  # Access to internal headers for testing
# )
#
# # Discover tests for CTest
# gtest_discover_tests(bolt_unit_tests
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#     PROPERTIES
#         LABELS "unit"
# )

# ============================================================================
# Integration Tests
# ============================================================================

set(INTEGRATION_TEST_SOURCES
    # integration/test_csp_solving.cpp
    # integration/test_examples.cpp
)

# Uncomment when test files are created
# add_executable(bolt_integration_tests ${INTEGRATION_TEST_SOURCES})
#
# target_link_libraries(bolt_integration_tests
#     PRIVATE
#         Bolt::Core
#         GTest::gtest
#         GTest::gtest_main
# )
#
# gtest_discover_tests(bolt_integration_tests
#     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#     PROPERTIES
#         LABELS "integration"
# )

# ============================================================================
# Test Coverage (Optional)
# ============================================================================

if(CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_COMPILER_IS_GNUCXX)
    option(ENABLE_COVERAGE "Enable code coverage" OFF)

    # if(ENABLE_COVERAGE)
    #     target_compile_options(bolt_unit_tests PRIVATE --coverage)
    #     target_link_options(bolt_unit_tests PRIVATE --coverage)
    #
    #     target_compile_options(bolt_integration_tests PRIVATE --coverage)
    #     target_link_options(bolt_integration_tests PRIVATE --coverage)
    # endif()
endif()
