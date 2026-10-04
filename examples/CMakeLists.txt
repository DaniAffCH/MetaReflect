add_executable(debug_print debug_print.cpp)
target_link_libraries(debug_print PRIVATE metaref::metaref)

if(METAREF_BUILD_TESTS)
    add_test(NAME example_debug_print COMMAND debug_print)
endif()