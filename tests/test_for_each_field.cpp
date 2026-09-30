#include <metaref/metaref.hpp>

#include <cstdio>
#include <string>
#include <string_view>
#include <format>

namespace{

int failures = 0;

void check(bool ok, std::string_view what) {
    if (!ok) {
        ++failures;
        std::printf("FAILED: %.*s\n", static_cast<int>(what.size()), what.data());
    }
}

struct Point { int x; int y; };

static_assert(metaref::field<^^Point::x>::name == "x");
static_assert(metaref::field<^^Point::y>::name == "y");

void test_sum() {
    Point p{3,4};

    int sum = 0;

    metaref::for_each_field(p, [&](auto, const auto& value ){ sum+=value; });

    check(sum==7, "test_sum");
}

void test_names() {
    Point p{3,4};
    std::string out;

    metaref::for_each_field(p, [&](auto field, const auto& ){ out += std::format("{};", field.name);});

    check(out == "x;y;", "test_names");
}

void test_const() {
    const Point p{1,2};
    std::string out; 

    metaref::for_each_field(p, [&](auto field, const auto& value){ out += std::format("{} = {}; ", field.name, value); });

    check(out == "x = 1; y = 2; ", "test_const");
}


}

int main() {
    test_sum();
    test_names();
    test_const();


    return failures > 0;
}
