#pragma once

#include <meta>
#include <string_view>

namespace metaref {

template <std::meta::info M>
struct field {
    static constexpr std::string_view name = std::meta::identifier_of(M); // This can be always used 
    static constexpr std::meta::info reflection = M; // This can be used only in a constant context
};

}