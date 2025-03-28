#pragma once

#include <algorithm>
#include <source_location>
#include <string>
#include <unordered_map>
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
struct none_sameLastLevelTypeName {
    static consteval bool operator()() {
        std::vector<std::string> vect;
        (vect.push_back(TypeToString<Ts>()), ...);
        std::ranges::sort(vect, std::less());
        auto [beg, end] = std::ranges::unique(vect);
        vect.erase(beg, end);
        return vect.size() == sizeof...(Ts);
    }
};

// Just the 'last level' type name ... not the fully qualified typename
template <typename... Ts>
concept none_sameLastLevelTypeName_v = none_sameLastLevelTypeName<Ts...>::operator()();

template <typename BASE, typename... Ts>
requires(std::is_base_of_v<BASE, Ts>, ...) && detail::none_sameLastLevelTypeName_v<Ts...>
constexpr inline auto generate_variantTypeMap() {
    std::unordered_map<std::string, std::variant<Ts...>> res;
    (res.insert({detail::TypeToString<Ts>(), std::variant<Ts...>(Ts())}), ...);
    return res;
}
} // namespace detail
} // namespace terminal_plot
} // namespace incom