#pragma once
#include <metaref/field.hpp>
#include <metaref/fields.hpp>

#include <meta>
#include <string_view>
#include <type_traits>
#include <optional>

namespace metaref {
    template <std::meta::access_context Ctx = std::meta::access_context::current(),
          typename T, typename F>
    bool visit_field(T& obj, std::string_view name, F&& f){
        using U = std::remove_cvref_t<T>;
        template for (constexpr std::meta::info member : fields_of(^^U, Ctx)) {
            field<member> the_field;
            if (the_field.name == name){
                f(the_field, obj.[:member:]);
                return true;
            }
        }
        return false;
    }

    template <class V,
            std::meta::access_context Ctx = std::meta::access_context::current(),
            typename T>
    std::optional<V> get_field(const T& obj, std::string_view name){
        std::optional<V> result;
        visit_field<Ctx>(obj, name, [&](auto, const auto& value) { 
            if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, V>) result = value;
        });

        return result;
    }
}