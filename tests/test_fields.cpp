#include <metaref/metaref.hpp>

#include <cstddef>
#include <initializer_list>
#include <span>
#include <string_view>

namespace {

consteval bool names_are(std::span<const std::meta::info> fields, std::initializer_list<std::string_view> expected) {
    if (fields.size() != expected.size()) return false;
    std::size_t i = 0;
    for (std::string_view name : expected) {
        if (std::meta::identifier_of(fields[i++]) != name) return false;
    }
    return true;
}

struct Point { int x; int y; };
static_assert(names_are(metaref::fields_of(^^Point), {"x", "y"}));

struct Empty {};
static_assert(metaref::fields_of(^^Empty).empty());

class WithPrivate {
    int hidden = 0;
public:
    int visible = 0;
    static consteval std::size_t count_from_inside() {
        return metaref::fields_of(^^WithPrivate).size();
    }

    static consteval bool complete_from_inside() {
        return metaref::fields_are_complete(^^WithPrivate);
    }
};

// only the public attribute from outside
static_assert(names_are(metaref::fields_of(^^WithPrivate), {"visible"}));
// also the private attribute from inside
static_assert(WithPrivate::count_from_inside() == 2);

struct Derived : Point { int z; };
// no derived attributes
static_assert(names_are(metaref::fields_of(^^Derived), {"z"}));

struct Tricky {
    int a;
    unsigned flag : 1;          
    unsigned : 3;             
    union { int i; float f; }; 
    int b;
};

// skip anonymous attributes
static_assert(names_are(metaref::fields_of(^^Tricky), {"a", "flag", "b"}));

} 

// template for works as the return type is static
int main() {
    Point p{3, 4};
    int sum = 0;
    template for (constexpr auto m : metaref::fields_of(^^Point)) {
        sum += p.[:m:];
    }
    return sum == 7 ? 0 : 1; 
}

static_assert(metaref::fields_are_complete(^^Point));
static_assert(metaref::fields_are_complete(^^Empty));
static_assert(!metaref::fields_are_complete(^^WithPrivate)); // hidden is not visible outside ...
static_assert(WithPrivate::complete_from_inside()); // ...but it is from inside
static_assert(!metaref::fields_are_complete(^^Tricky)); // anonymous union
static_assert(!metaref::fields_are_complete(^^Derived)); // inherited x, y

struct EmptyTag {};
struct Tagged : EmptyTag { int v; };

static_assert(metaref::fields_are_complete(^^Tagged)); // Derived but with an empty base class. Shoulf return true.

struct PrivatelyTagged : private EmptyTag { int v; };
struct HidesData : private Point { int v; };

static_assert(metaref::fields_are_complete(^^PrivatelyTagged));
static_assert(!metaref::fields_are_complete(^^HidesData)); 