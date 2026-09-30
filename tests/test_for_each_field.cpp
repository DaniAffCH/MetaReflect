#include <metaref/for_each_field.hpp>

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

struct FunctorCounter {
    int calls = 0;
    void operator()(auto, const auto&) { ++calls; }
};

void test_callback_not_copied(){
    Point p{1,2};
    FunctorCounter c;
    metaref::for_each_field(p,c);

    check(c.calls==2, "test_callback_not_copied");
}

void test_modification() {
    Point p{1, 2};
    metaref::for_each_field(p, [](auto, auto& value) { value += 10; });
    check(p.x == 11 && p.y == 12, "test_modification");
}

struct Flags {
    unsigned ready : 1;
    unsigned mode  : 3;
};

void test_bitfield_read() {
    Flags fl{1, 5};
    unsigned sum = 0;
    metaref::for_each_field(fl, [&](auto, const auto& value) { sum += value; });
    check(sum == 6, "test_bitfield_read");
}

class Secret{
    private:
    int hidden = 1;
    public:
    int visible = 1;

    int count_from_inside(){
        int n=0;
        metaref::for_each_field(*this, [&](auto, const auto&){ ++n; });
        return n;
    }
};

void test_access(){
    Secret s;
    int outside = 0;
    metaref::for_each_field(s, [&](auto, const auto&){ ++outside; });
    int inside = s.count_from_inside();

    check(outside == 1, "test_access");
    check(inside == 2, "test_access");
}

struct WithUnion {
    int a;
    union { int i; float f; };
    int b;
};

void test_anon_names(){
    WithUnion p{};
    std::string out;

    metaref::for_each_field(p, [&](auto field, const auto& ){ out += std::format("{};", field.name);});

    check(out == "a;b;", "test_anon_names");
}

struct Derived : Point { int z; };

void test_derived(){
    Derived d{1,2,3};
    std::string out;

    metaref::for_each_field(d, [&](auto field, const auto& ){ out += std::format("{};", field.name);});

    check(out == "z;", "test_derived");
}

}

int main() {
    test_sum();
    test_names();
    test_const();
    test_callback_not_copied();
    test_modification();
    test_bitfield_read();
    test_access();
    test_anon_names();
    test_derived();

    return failures > 0;
}
