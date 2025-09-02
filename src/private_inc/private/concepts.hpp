#pragma once

#include <typeindex>
#include <variant>

#include <incstd/variant_utils.hpp>
#include <incstd/views.hpp>


namespace incom {
namespace terminal_plot {
namespace detail {
// Quasi compile time reflection for typenames
template <typename T>
constexpr auto get_typeIndex() {
    return std::type_index(typeid(T));
}
template <typename T>
constexpr auto get_typeIndex(T) {
    return std::type_index(typeid(T));
}

template <typename BASE, typename... Ts>
requires(std::is_base_of_v<BASE, Ts>, ...) && incom::standard::concepts::types_noneSame_v<Ts...>
struct VariantTypeMap {
    // PTC = Pass To Constructors
    template <typename... PTC>
    static constexpr inline auto gen_typeMap(PTC const &...ptc) {
        // return incom::standard::variant_utils::VariantUtility<Ts...>::gen_typeMap(ptc...);
        std::unordered_map<std::type_index, const std::variant<Ts...>> res;
        (res.insert({incom::standard::typegen::get_typeIndex<Ts>(),
                     std::variant<Ts...>(Ts(std::forward<decltype(ptc)>(ptc)...))}),
         ...);
        return res;
    };
};

template <typename T>
concept is_someVariant =
    requires(T t) { std::is_same_v<std::variant_alternative_t<0, T>, std::variant_alternative_t<0, T>>; };


} // namespace detail
} // namespace terminal_plot
} // namespace incom