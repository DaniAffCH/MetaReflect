#pragma once

#include <meta>
#include <string_view>
#include <type_traits>

namespace metaref {

// Returns the name of the enumerator with the same value as value. If multiple enumerators have the same value the first one wins.
// Returns an empty string_view if no enumerator matches
template <class T> 
    requires std::is_enum_v<T>
constexpr std::string_view enum_name(T value){
    template for (constexpr std::meta::info member : std::define_static_array(std::meta::enumerators_of(^^T))) {
        constexpr T candidate = [:member:];
        if(value == candidate)
            return std::meta::identifier_of(member);
    }

    return "";
}

}