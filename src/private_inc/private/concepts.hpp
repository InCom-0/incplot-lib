#pragma once

#include <array>
#include <incstd/views.hpp>
#include <source_location>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>


namespace incom {
namespace terminal_plot {
namespace detail {
// Quasi compile time reflection for typenames
template <typename T>
constexpr auto TypeToString() {
    auto EmbeddingSignature = std::string{std::source_location::current().function_name()};
    auto firstPos           = EmbeddingSignature.rfind("::") + 2;
    return EmbeddingSignature.substr(firstPos, EmbeddingSignature.size() - firstPos - 1);
}

template <typename... Ts>
struct _types_noneSame {
    static consteval bool operator()() {
        std::vector<const std::type_info *> typeVect{&typeid(Ts)...};
        for (auto [lhs, rhs] : incom::standard::views::combinations_k<2uz>(typeVect)) {
            if (*lhs == *rhs) { return false; }
        }
        return true;
    }
};

// Just the 'last level' type name ... not the fully qualified typename
template <typename... Ts>
concept types_noneSame_v = _types_noneSame<Ts...>::operator()();

template <typename BASE, typename... Ts>
requires(std::is_base_of_v<BASE, Ts>, ...) && detail::types_noneSame_v<Ts...>
struct VariantUtility {
    // PTC = Types To Pass To Constructors
    template <typename... PTC>
    static constexpr inline auto gen_typeMap(PTC const &...ptc) {
        std::unordered_map<std::string, std::variant<Ts...>> res;
        (res.insert({detail::TypeToString<Ts>(), std::variant<Ts...>(Ts(std::forward<decltype(ptc)>(ptc)...))}), ...);
        return res;
    };

    static constexpr inline auto gen_typeMap() {
        std::unordered_map<std::string, std::variant<Ts...>> res;
        (res.insert({detail::TypeToString<Ts>(), std::variant<Ts...>(Ts())}), ...);
        return res;
    };
};

template <typename T>
concept is_someVariant =
    requires(T t) { std::is_same_v<std::variant_alternative_t<0, T>, std::variant_alternative_t<0, T>>; };


} // namespace detail
} // namespace terminal_plot
} // namespace incom