add_executable(test_fields test_fields.cpp)
target_link_libraries(test_fields PRIVATE metaref::metaref)
set_target_properties(test_fields PROPERTIES CXX_EXTENSIONS OFF)
add_test(NAME fields COMMAND test_fields)