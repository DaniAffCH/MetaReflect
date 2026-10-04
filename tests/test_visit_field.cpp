#include <metaref/visit_field.hpp>

#include <cstdio>
#include <string_view>
#include <optional>

namespace {

int failures = 0;

void check(bool ok, std::string_view what) {
    if (!ok) {
        ++failures;
        std::printf("FAILED: %.*s\n", static_cast<int>(what.size()), what.data());
    }
}

struct Point { int x; int y; };

void test_found() {
    Point p{1, 2};
    int seen = 0;
    bool found = metaref::visit_field(p, "y", [&](auto, const auto& value) { seen = value; });
    check(found, "test_found");
    check(seen == 2, "test_found");
}

void test_not_found() {
    Point p{1, 2};
    int seen = 0;
    bool found = metaref::visit_field(p, "z", [&](auto, const auto& value) { seen = value; });
    check(!found, "test_not_found");
    check(seen == 0, "test_not_found");
}

void test_modification() {
    Point p{1, 2};
    bool found = metaref::visit_field(p, "x", [&](auto, auto& value) { value = 100; });
    check(found, "test_modification");
    check(p.x == 100, "test_modification");
    check(p.y == 2, "test_modification");

}

class Secret {
    int hidden = 1;
public:
    int visible = 2;

    bool read_hidden_from_inside() {
        int seen = 0;
        bool found = metaref::visit_field(*this, "hidden", [&](auto, const auto& value) { seen = value; });
        return found && seen == 1;
    }
};

void test_access() {
    Secret s;
    bool from_outside = metaref::visit_field(s, "hidden", [](auto, const auto&) {});
    check(!from_outside, "test_access");
    check(s.read_hidden_from_inside(), "test_access");
}

struct Config {
    int port = 8080;
    std::string host = "localhost";
    double ratio = 2.7;
};

void test_get_field_int() {
    Config c;
    std::optional<int> port = metaref::get_field<int>(c, "port");
    check(port.has_value() && *port == 8080, "test_get_field_int");
}

void test_get_field_string() {
    Config c;
    std::optional<std::string> host = metaref::get_field<std::string>(c, "host");
    check(host.has_value() && *host == "localhost", "test_get_field_string");
}

void test_get_field_wrong_type() {
    Config c;
    check(!metaref::get_field<int>(c, "host").has_value(), "test_get_field_wrong_type");
    check(!metaref::get_field<int>(c, "ratio").has_value(), "test_get_field_wrong_type");
    check(!metaref::get_field<long>(c, "port").has_value(), "test_get_field_wrong_type");
}

void test_get_field_not_found() {
    Config c;
    check(!metaref::get_field<int>(c, "prot").has_value(), "test_get_field_not_found");
}

class Vault {
    int code = 42;
public:
    int id = 1;

    std::optional<int> code_from_inside() const {
        return metaref::get_field<int>(*this, "code");
    }
};

void test_get_field_access() {
    Vault v;
    check(!metaref::get_field<int>(v, "code").has_value(), "test_get_field_access");
    std::optional<int> inside = v.code_from_inside();
    check(inside.has_value() && *inside == 42,"test_get_field_access");
}

}

int main() {
    test_found();
    test_not_found();
    test_modification();
    test_get_field_int();
    test_get_field_string();
    test_get_field_wrong_type();
    test_get_field_not_found();
    test_get_field_access();
    return failures > 0;
}