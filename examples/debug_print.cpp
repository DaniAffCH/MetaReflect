#include <metaref/metaref.hpp>
#include <string> 
#include <string_view>
 #include <format>
 #include <iostream>

struct Point { int x; int y; };

enum Color { Green = 1, Red = 2, Blue = 3 };

int main(){
    Point p{.x=1, .y=2};
    
    std::string out = "{";
    std::string_view sep = "";
    metaref::for_each_field(p, [&](auto field, const auto& value) {
        out += std::format("{}{}: {}", sep, field.name, value);
        sep = ", ";
    });
    out += "}";

    std::cout << "Point: " << out << std::endl; // Point: {x: 1, y: 2}

    Color c = Red;

    std::cout << "Color: "<< metaref::enum_name(c) << std::endl; // Color: Red

    return 0;
}