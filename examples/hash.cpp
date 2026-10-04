#include <metaref/metaref.hpp>

#include <cstddef>
#include <iostream>
#include <meta>
#include <string>
#include <unordered_map>

// Hash every field of obj
template <std::meta::access_context Ctx = std::meta::access_context::current(), class T>
std::size_t hash_value(const T& obj) {
    static_assert(metaref::fields_are_complete(^^T, Ctx), "hash_value needs every field of T, but some are not visible from the caller");

    std::size_t h = 0;
    metaref::for_each_field<Ctx>(obj, [&](auto, const auto& value) {
        using V = std::remove_cvref_t<decltype(value)>;
        // hash_combine mixing step (as in Boost)
        h ^= std::hash<V>{}(value) + 0x9e3779b9 + (h << 6) + (h >> 2);
    });
    return h;
}

// required to hash a key in std::unordered_map.
struct field_hasher {
    template <class T>
    std::size_t operator()(const T& obj) const {
        return hash_value(obj);
    }
};

struct Point {
    int x;
    int y;
    bool operator==(const Point&) const = default;
};

class Account {
private:
    long balance;
public:
    int id;
    Account(int id, long balance) : balance(balance), id(id) {}
    std::size_t hash() const { return hash_value(*this); }
};

int main() {
    std::unordered_map<Point, std::string, field_hasher> labels;
    labels[{1, 2}] = "start";
    labels[{3, 4}] = "end";
    std::cout << "labels.at({3, 4}) = " << labels.at({3, 4}) << '\n';

    Account a{7, 100};
    Account b{7, 200};
    std::cout << "a.hash() != b.hash(): " << std::boolalpha << (a.hash() != b.hash()) << '\n';

    // hash_value(a) doesn't compile becuase from here balance is private.
    // Bypassing access is possible, but it is a deliberate choice that ignores the encapsulation.
    // hash_value<std::meta::access_context::unchecked()>(a)

    return 0;
}