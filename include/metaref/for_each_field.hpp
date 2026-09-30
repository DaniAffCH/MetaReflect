#pragma once

#include <metaref/fields.hpp>
#include <meta>
#include <cstddef>

namespace metaref {


template <std::meta::info M>
struct field {
    static constexpr std::string_view name = std::meta::identifier_of(M); // This can be always used 
    static constexpr std::meta::info reflection = M; // This can be used only in a constant context
};

template <std::meta::access_context Ctx = std::meta::access_context::current(), class T, class F>
void for_each_field(T& obj, F&& f){
    using U = std::remove_cvref_t<T>;
    template for (constexpr std::meta::info member : fields_of(^^U, Ctx)) {
        f(field<member>{}, obj.[:member:]);
    }

}

}