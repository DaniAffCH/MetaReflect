cmake_minimum_required(VERSION 3.25)
project(metaref VERSION 0.1.0 LANGUAGES CXX)

add_library(metaref INTERFACE)
add_library(metaref::metaref ALIAS metaref)

target_include_directories(metaref INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

if("cxx_std_26" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(metaref INTERFACE cxx_std_26)
else()
    message(STATUS "metaref: this CMake cannot request C++26 for ${CMAKE_CXX_COMPILER_ID} "
                   "${CMAKE_CXX_COMPILER_VERSION}; passing -std=c++26 directly")
    target_compile_options(metaref INTERFACE $<$<CXX_COMPILER_ID:GNU>:-std=c++26>)
endif()

target_compile_options(metaref INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-freflection>)

option(METAREF_BUILD_TESTS "Build metaref tests" ${PROJECT_IS_TOP_LEVEL})
if(METAREF_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(METAREF_BUILD_EXAMPLES "Build metaref examples" ${PROJECT_IS_TOP_LEVEL})
if(METAREF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()