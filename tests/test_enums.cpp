#include <metaref/enums.hpp>


namespace {

    enum Color { Green = 1, Red = 2, Blue = 3 };
 
    static_assert(metaref::enum_name(Green) == "Green");
    static_assert(metaref::enum_name(Red) == "Red");
    static_assert(metaref::enum_name(Blue) == "Blue");
 
    enum class Shade { red = 1, crimson = 1, green = 2, blue = 4 };
 
    static_assert(metaref::enum_name(Shade::green) == "green");
    static_assert(metaref::enum_name(Shade::crimson) == "red"); // first one declared win
    static_assert(metaref::enum_name(Shade{6}).empty());

    enum class Sparse : int { neg = -1, five = 5, hundred = 100, big = 1'000'000 };
 
    static_assert(metaref::enum_name(Sparse::neg) == "neg");
    static_assert(metaref::enum_name(Sparse::hundred) == "hundred");
    static_assert(metaref::enum_name(Sparse::big) == "big");

    struct Device { enum class State { off, on };};
 
    static_assert(metaref::enum_name(Device::State::off) == "off");
    static_assert(metaref::enum_name(Device::State::on) == "on");

    static_assert(metaref::enum_from_name<Shade>("green") == Shade::green);
    static_assert(metaref::enum_from_name<Shade>("crimson") == Shade::red); 
    static_assert(!metaref::enum_from_name<Shade>("purple").has_value());

    static_assert(metaref::enum_from_name<Color>("Red") == Color::Red); 

    static_assert(metaref::enum_count<Color> == 3);
    static_assert(metaref::enum_count<Shade> == 4);
}

int main(int argc, char**) {
    Shade s = (argc > 100) ? Shade::red : Shade::blue;
    if (metaref::enum_name(s) != "blue") return 1;

    std::string_view text = (argc > 100) ? "red" : "green";
    if (metaref::enum_from_name<Shade>(text) != Shade::green) return 1;

    return 0;
}