# metaref documentation

This page covers everything metaref offers. If you just want a quick look, the [README](../README.md) is enough.

## Headers

| Header | What's inside |
|---|---|
| `<metaref/fields.hpp>` | `fields_of`, `fields_are_complete` |
| `<metaref/field.hpp>` | `field<M>` |
| `<metaref/for_each_field.hpp>` | `for_each_field` |
| `<metaref/visit_field.hpp>` | `visit_field`, `get_field` |
| `<metaref/enums.hpp>` | `enum_name`, `enum_from_name`, `enum_count`, `enum_names`, `enum_values`, `enum_contains` |
| `<metaref/metaref.hpp>` | all of the above |

Reflection is slow to compile, so it's better to include only what you use.

## A few things to know first

### Compile time vs runtime

Reflection always happens at compile time, but the code it generates can run at runtime.

Functions that take or return a `std::meta::info` (`fields_of`, `fields_are_complete`) are `consteval`, since a reflection only exists inside the compiler. Everything else works on normal values and can be used at runtime. The `constexpr` ones can also be used in a `static_assert`.

Things that only depend on a type, like `enum_count`, are `inline constexpr` variables, the same way `std::is_enum_v` is.

### Access

Splices don't check access: `obj.[:m:]` can read a private member from anywhere. What decides which members you get is the `access_context` passed to the reflection query, so metaref never uses `unchecked()` on its own.

By default every function uses the context of whoever calls it. From inside a class you see its private fields, from outside you don't:

```cpp
class Account {
    long balance;
public:
    int id;

    void inside() {
        metaref::fields_of(^^Account);   // id and balance
    }
};

metaref::fields_of(^^Account);           // only id
```

If you really need to see everything, you can ask for it:

```cpp
metaref::fields_of(^^Account, std::meta::access_context::unchecked());
metaref::for_each_field<std::meta::access_context::unchecked()>(account, f);
```

Keep in mind that this ignores the encapsulation of the class.

`fields_of` and `fields_are_complete` take the context as a normal argument. `for_each_field`, `visit_field` and `get_field` take it as a template argument, because they run at runtime and a runtime parameter can't decide which fields to iterate.

### Complete and partial views

`fields_of` gives you the fields you can see, which is not always the whole object. Private fields seen from outside, unnamed members (like anonymous unions) and inherited fields are not there.

For printing this is fine. For something like a hash or a comparison it's not, because the result would be wrong without any warning. That's what `fields_are_complete` is for (see [Writing your own algorithms](#writing-your-own-algorithms)).

## Fields

### `fields_of`

```cpp
consteval std::span<const std::meta::info> fields_of(
    std::meta::info type,
    std::meta::access_context ctx = std::meta::access_context::current());
```

Returns the non-static data members of `type` that `ctx` can see and that have a name, in declaration order. Inherited members are not included.

The result is a static array, so you can use it directly in a `template for`:

```cpp
struct Point { int x; int y; };

template for (constexpr auto m : metaref::fields_of(^^Point)) {
    std::cout << std::meta::identifier_of(m) << '\n';   // x, y
}
```

### `fields_are_complete`

```cpp
consteval bool fields_are_complete(
    std::meta::info type,
    std::meta::access_context ctx = std::meta::access_context::current());
```

Returns `true` if `fields_of(type, ctx)` contains all the data of the object. It's `false` if some field is not visible from `ctx`, if there is an unnamed member, or if a base class (of any access) holds data. Empty bases don't count.

```cpp
struct Point   { int x; int y; };
struct Tag     {};
struct Tagged  : Tag   { int v; };
struct Derived : Point { int z; };

static_assert(metaref::fields_are_complete(^^Point));
static_assert(metaref::fields_are_complete(^^Tagged));     // empty base
static_assert(!metaref::fields_are_complete(^^Derived));   // x and y are inherited
```

### `field<M>`

```cpp
template <std::meta::info M>
struct field {
    static constexpr std::string_view name;
    static constexpr std::meta::info reflection;
};
```

This is what callbacks receive to know which field they are looking at. A reflection can't be passed around at runtime, so the information lives in the type and the object itself is empty.

`name` can be used anywhere. `reflection` only in a constant context (`if constexpr`, `static_assert`, ...), since it doesn't exist at runtime.

### `for_each_field`

```cpp
template <std::meta::access_context Ctx = std::meta::access_context::current(),
          typename T, typename F>
void for_each_field(T& obj, F&& f);
```

Calls `f(field, value)` for every field in `fields_of(^^T, Ctx)`.

```cpp
Point p{1, 2};

metaref::for_each_field(p, [](auto field, const auto& value) {
    std::cout << field.name << " = " << value << '\n';   // x = 1, y = 2
});

metaref::for_each_field(p, [](auto, auto& value) { value += 10; });   // p is now {11, 12}
```

Some notes:

- The callback is compiled once for each field, so it has to work with all the field types. Use `if constexpr` if you need to handle them differently.
- It doesn't check completeness: it visits what you can see.
- The callback is not copied, so a function object with state keeps it after the call.
- Bit-fields can be read through `const auto&`, but not taken by non-const reference.
- Temporaries are not accepted. Give the object a name first.
- `const` doesn't propagate through reference members.
- You can't stop in the middle. If you need `break`, use a `template for` over `fields_of`.

## Access by name

### `visit_field`

```cpp
template <std::meta::access_context Ctx = std::meta::access_context::current(),
          typename T, typename F>
bool visit_field(T& obj, std::string_view name, F&& f);
```

Looks for the field called `name` and calls `f(field, value)` on it. Returns `true` if it found it, `false` otherwise.

Since `name` is only known at runtime, the compiler can't know which field you'll get, and so it can't know the type to return. That's why it takes a callback instead of returning the value.

```cpp
struct Settings { int width; int height; };

Settings s{800, 600};
metaref::visit_field(s, "width", [](auto, auto& value) { value = 1024; });   // s.width == 1024
```

A field you can't see behaves like a field that doesn't exist. Like with `for_each_field`, the callback has to compile for every field type.

### `get_field`

```cpp
template <class V,
          std::meta::access_context Ctx = std::meta::access_context::current(),
          typename T>
std::optional<V> get_field(const T& obj, std::string_view name);
```

If you already know the type of the field, this is simpler. It returns a copy of the field called `name` if it exists and its type is exactly `V`, otherwise `std::nullopt`. There are no conversions, so a `double` will never be truncated into an `int`.

```cpp
struct Config { int port; std::string host; double ratio; };

Config c{8080, "localhost", 2.7};

metaref::get_field<int>(c, "port");           // 8080
metaref::get_field<std::string>(c, "host");   // "localhost"
metaref::get_field<int>(c, "host");           // nullopt, wrong type
metaref::get_field<int>(c, "ratio");          // nullopt, no truncation
metaref::get_field<int>(c, "prot");           // nullopt, no such field
```

## Enums

These work with both `enum class` and plain enums, with any values (negative, sparse, big) and without range limits. Passing something that is not an enum doesn't compile.

When several enumerators have the same value (aliases), every enumerator counts on its own, in declaration order. The examples use:

```cpp
enum class Shade { red = 1, crimson = 1, green = 2, blue = 4 };
```

### `enum_name`

```cpp
template <typename T> requires std::is_enum_v<T>
constexpr std::string_view enum_name(T value);
```

Returns the name of the enumerator with that value. With aliases the first declared one wins. If nothing matches, it returns an empty string.

```cpp
metaref::enum_name(Shade::green);     // "green"
metaref::enum_name(Shade::crimson);   // "red", same value and red comes first
metaref::enum_name(Shade{6});         // ""
```

### `enum_from_name`

```cpp
template <typename T> requires std::is_enum_v<T>
constexpr std::optional<T> enum_from_name(std::string_view name);
```

Returns the enumerator called `name`, or `std::nullopt`. Aliases are accepted.

```cpp
metaref::enum_from_name<Shade>("green");     // Shade::green
metaref::enum_from_name<Shade>("crimson");   // same value as Shade::red
metaref::enum_from_name<Shade>("purple");    // nullopt
```

Why a string in one case and an `optional` in the other? A name can never be empty, so an empty string is a safe way to say "not found". A value can't do the same, because any value of the enum could be a real enumerator.

### `enum_count`

```cpp
template <typename T> requires std::is_enum_v<T>
inline constexpr std::size_t enum_count;
```

The number of enumerators, aliases included. `enum_count<Shade>` is 4.

### `enum_names`

```cpp
template <typename T> requires std::is_enum_v<T>
inline constexpr std::array<std::string_view, enum_count<T>> enum_names;
```

All the names in declaration order, aliases included. These are exactly the names `enum_from_name` accepts.

```cpp
for (std::string_view n : metaref::enum_names<Shade>) {
    std::cout << n << '\n';   // red, crimson, green, blue
}
```

### `enum_values`

```cpp
template <typename T> requires std::is_enum_v<T>
inline constexpr std::array<T, enum_count<T>> enum_values;
```

All the values in declaration order. With aliases, the same value appears more than once.

`enum_names` and `enum_values` go together: index `i` is the same enumerator in both. Be careful with aliases though, since `enum_name` looks up by value:

```cpp
static_assert(metaref::enum_names<Shade>[1] == "crimson");
static_assert(metaref::enum_name(metaref::enum_values<Shade>[1]) == "red");
```

### `enum_contains`

```cpp
template <typename T> requires std::is_enum_v<T>
constexpr bool enum_contains(T value);
```

Tells you if `value` matches one of the enumerators. Handy to check an integer you just casted to an enum.

```cpp
metaref::enum_contains(Shade::green);   // true
metaref::enum_contains(Shade{3});       // false
```

## Writing your own algorithms

If you build something on top of metaref, two things matter.

If your algorithm needs the whole object, check `fields_are_complete`, otherwise it will just skip the fields it can't see.

Also, forward the access context. Give your function a `Ctx` that defaults to `current()` and pass it explicitly to the metaref functions you call. If you don't, they will take the context of your function instead of the caller's, and a call from inside a class won't see its private fields anymore.

```cpp
template <std::meta::access_context Ctx = std::meta::access_context::current(), class T>
std::size_t hash_value(const T& obj) {
    static_assert(metaref::fields_are_complete(^^T, Ctx),
                  "hash_value needs every field of T, but some are not visible from here");

    std::size_t h = 0;
    metaref::for_each_field<Ctx>(obj, [&](auto, const auto& value) {
        using V = std::remove_cvref_t<decltype(value)>;
        h ^= std::hash<V>{}(value) + 0x9e3779b9 + (h << 6) + (h >> 2);
    });
    return h;
}
```

The full example is in [examples/hash.cpp](../examples/hash.cpp).

## Limitations

- Only GCC 16 for now (`-std=c++26 -freflection`).
- Inherited fields are not visited.
- The default access context remembers where it was created, so calling the same function from different places creates different instantiations. This costs some compile time.
- On GCC, a default template argument `current()` gets the caller's context, and `for_each_field`, `visit_field` and `get_field` rely on this. I haven't found it confirmed in the standard yet (for default function arguments it is).