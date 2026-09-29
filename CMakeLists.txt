cmake_minimum_required(VERSION 3.25)
project(metaref VERSION 0.1.0 LANGUAGES CXX)

# Header-only library: nothing to compile, only usage requirements
# (include path, C++ standard, flags) that propagate to whoever links it.
add_library(metaref INTERFACE)
add_library(metaref::metaref ALIAS metaref)

target_include_directories(metaref INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# Prefer the portable way. Older CMake releases know the name "cxx_std_26" but
# not which flag a newer compiler needs for it; in that case pass the flag ourselves.
if("cxx_std_26" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(metaref INTERFACE cxx_std_26)
else()
    message(STATUS "metaref: this CMake cannot request C++26 for ${CMAKE_CXX_COMPILER_ID} "
                   "${CMAKE_CXX_COMPILER_VERSION}; passing -std=c++26 directly")
    target_compile_options(metaref INTERFACE $<$<CXX_COMPILER_ID:GNU>:-std=c++26>)
endif()

# GCC 16 needs an explicit flag to enable reflection.
target_compile_options(metaref INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-freflection>)

option(METAREF_BUILD_TESTS "Build metaref tests" ${PROJECT_IS_TOP_LEVEL})
if(METAREF_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
