# MetaRef

[![CI](https://github.com/DaniAffCH/metaref/actions/workflows/ci.yml/badge.svg)](https://github.com/DaniAffCH/metaref/actions/workflows/ci.yml)

This library is meant to make the use of C++26 static reflection less painful. 

[P2996](https://isocpp.org/files/papers/P2996R13.html) static reflection is still at an early stage and its use can be quite error-prone given the plethora of possible edge cases.

`metaref` provides a collection of STL-like utilities mostly built on top of `std::meta` with the goal of handling these cases internally and providing a simple, centralized API.

Full documentation: [docs/reference.md](docs/reference.md)

> Status: experimental. metaref is at a very early stage and is developed against GCC 16 with '-freflection'. The API will change.

## Quickstart

```cpp
struct Point { int x; int y; };

Point p{1, 2};
metaref::for_each_field(p, [](auto field, const auto& value) {
    std::cout << field.name << " = " << value << '\n';   // x = 1, y = 2
});
```

## Using metaref

metaref is header-only:
```cmake
add_subdirectory(metaref)
target_link_libraries(your_target PRIVATE metaref::metaref)
```

Linking metaref::metaref adds the include path, C++26 and `-freflection`.

## Why should I use metaref

`std::meta` gives you the primitives, but using them correctly takes more than it looks and it's easy to overlook nuances. metaref:
- handles the edge cases: private members, anonymous unions, bit-fields, base classes, enum aliases
- hides most of the ugly syntax: static arrays, access contexts, splices.  
- gives you results you can use at runtime, not only inside the compiler
- respects encapsulation by default
- stays small: no macros, no formats, no registration

## Building the tests and examples
```bash
cmake -B build -DCMAKE_CXX_COMPILER=g++-16
cmake --build build
ctest --test-dir build --output-on-failure
```

Most tests are static_asserts, so a failing test shows up as a compile error.

## Requirements
- GCC 16 with -std=c++26 -freflection
- CMake 3.25 or later