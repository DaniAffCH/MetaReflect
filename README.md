# MetaReflect

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

## What's included?

### Fields

* `metaref::fields_of`
* `metaref::fields_are_complete`
* `metaref::field`
* `metaref::for_each_field`
* `metaref::visit_field`
* `metaref::get_field`

For example, `fields_are_complete` can check that reflection gives you all the fields you expect:

```cpp
static_assert(metaref::fields_are_complete(^^Point));
```

`for_each_field` handles the reflection machinery needed to associate reflected members with their values:

```cpp
metaref::for_each_field(object, [](auto field, const auto& value) {
    // ...
});
```

It also preserves the caller's access context instead of simply using `unchecked()`.

Runtime field access is available through `visit_field` and `get_field`:

```cpp
if (auto value = metaref::get_field<int>(object, "x")) {
    std::cout << *value << '\n';
}
```

### Enums

* `metaref::enum_name`
* `metaref::enum_from_name`
* `metaref::enum_count`
* `metaref::enum_names`
* `metaref::enum_values`
* `metaref::enum_contains`

These support sparse and negative enum values and handle aliases explicitly.

```cpp
enum class Color {
    red = -2,
    green = 1,
    blue = 4
};

metaref::enum_name(Color::green);
metaref::enum_values<Color>();
metaref::enum_names<Color>();
```

## Writing your own utilities

MetaReflect is also meant to be used as a building block for higher-level reflection utilities.

For example:

```cpp
template <typename T>
std::size_t hash_value(const T& object) {
    static_assert(metaref::fields_are_complete(^^T));

    std::size_t hash = 0;

    metaref::for_each_field(object, [&](auto, const auto& value) {
        hash_combine(hash, value);
    });

    return hash;
}
```

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

## Examples
See the [examples](examples/) directory.

  
