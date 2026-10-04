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

}

int main(){
    return 0;
}