#pragma once

#include <meta>
#include <span>
#include <vector>

namespace metaref {

// Returns a static array containing all the non static data members of type that have a name.
// All inherited members are not included. The default current context is evaluated at the call place.
consteval std::span<const std::meta::info> fields_of(std::meta::info type, std::meta::access_context ctx = std::meta::access_context::current())
{
    std::vector<std::meta::info> result;

    for(std::meta::info member : std::meta::nonstatic_data_members_of(type, ctx)){
        if(std::meta::has_identifier(member)){
            result.push_back(member);
	    }   
    }

    return std::define_static_array(result);
}

// True if type stores any non-static data member (inherited members included)
consteval bool holds_data(std::meta::info type) {
    if (!std::meta::nonstatic_data_members_of(type, std::meta::access_context::unchecked()).empty()) {
        return true;
    }
    for (std::meta::info base : std::meta::bases_of(type, std::meta::access_context::unchecked())) {
        if (holds_data(std::meta::type_of(base))) {
            return true;
        }
    }
    return false;
}

// Returns true if fields_of lists every piece of data of type
consteval bool fields_are_complete(std::meta::info type, std::meta::access_context ctx = std::meta::access_context::current()){
    // field hidden from the caller
    if (std::meta::has_inaccessible_nonstatic_data_members(type, ctx)) 
        return false;

    // An anonymous field
    for(std::meta::info member : std::meta::nonstatic_data_members_of(type, ctx)){
        if(!std::meta::has_identifier(member))
            return false;
    }

    // Inherited fields (that is, there is at least a base non-empty)
    for (std::meta::info base : std::meta::bases_of(type, std::meta::access_context::unchecked())) {
        if (holds_data(std::meta::type_of(base))) 
            return false;
    }

    return true;
}

}