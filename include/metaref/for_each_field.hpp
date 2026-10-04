#pragma once

#include <metaref/field.hpp>
#include <metaref/fields.hpp>
#include <meta>
#include <type_traits>
namespace metaref {

template <std::meta::access_context Ctx = std::meta::access_context::current(), typename T, typename F>
void for_each_field(T& obj, F&& f){
    using U = std::remove_cvref_t<T>;
    template for (constexpr std::meta::info member : fields_of(^^U, Ctx)) {
        f(field<member>{}, obj.[:member:]);
    }

}

}