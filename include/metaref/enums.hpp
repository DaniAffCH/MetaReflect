#pragma once

#include <array>
#include <meta>
#include <optional>
#include <string_view>
#include <type_traits>
#include <cstddef>
namespace metaref {

// Returns the name of the enumerator with the same value as value. If multiple enumerators have the same value the first one wins.
// Returns an empty string_view if no enumerator matches
template <typename T> 
    requires std::is_enum_v<T>
constexpr std::string_view enum_name(T value){
    template for (constexpr std::meta::info member : std::define_static_array(std::meta::enumerators_of(^^T))) {
        constexpr T candidate = [:member:];
        if(value == candidate)
            return std::meta::identifier_of(member);
    }

    return "";
}

// Returns the enumerator corresponding to name if it exists.
template <typename T>
    requires std::is_enum_v<T>
constexpr std::optional<T> enum_from_name(std::string_view name){
    template for (constexpr std::meta::info member : std::define_static_array(std::meta::enumerators_of(^^T))) {
        constexpr T candidate = [:member:];
        if (std::meta::identifier_of(member) == name)
            return candidate;
    }

    return std::nullopt;
}

template <typename T>
    requires std::is_enum_v<T>
constexpr bool enum_contains(T value){
    return !metaref::enum_name(value).empty();
}

template <typename T>
    requires std::is_enum_v<T>
constexpr inline std::size_t enum_count = std::meta::enumerators_of(^^T).size();

template <typename T>
    requires std::is_enum_v<T>
constexpr inline std::array<std::string_view, enum_count<T>> enum_names = []{
    std::array<std::string_view, enum_count<T>> names{};

    std::size_t i = 0;
    for (std::meta::info member : std::meta::enumerators_of(^^T)) {
        names[i++] = std::meta::identifier_of(member);
    }

    return names;
}();

template <typename T>
    requires std::is_enum_v<T>
constexpr inline std::array<T, enum_count<T>> enum_values = []{
    std::array<T, enum_count<T>> values{};

    std::size_t i = 0;
    for (std::meta::info member : std::meta::enumerators_of(^^T)) {
        values[i++] = std::meta::extract<T>(member);
    }

    return values;
}();


}