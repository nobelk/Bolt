# ============================================================================
# Bolt Examples
# ============================================================================

# Helper function to create examples
function(add_bolt_example name source)
    # Uncomment when implementation files are ready
    # add_executable(${name} ${source})
    # target_link_libraries(${name} PRIVATE Bolt::Core)
    # set_target_properties(${name} PROPERTIES
    #     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    # )
endfunction()

# Create example executables (uncomment when ready)
# add_bolt_example(simple_csp simple_csp.cpp)
# add_bolt_example(n_queens n_queens.cpp)
# add_bolt_example(map_coloring map_coloring.cpp)
