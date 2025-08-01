#pragma once

#include <cmath>
#include <format>
#include <functional>
#include <iterator>
#include <ranges>
#include <type_traits>

#include <private/color_mixer.hpp>
#include <private/concepts.hpp>
#include <ww898/utf_converters.hpp>


namespace incom {
namespace terminal_plot {
namespace detail {

// Auto 'bind_back', for some reason I couldn't manage to get std::bind_back to work ... this works fine :-)
// Returns a lambda (closure) which invokes 'fn' passed in as first argument with the last 'size_of(Ts)' arguments
// bound to 'ts' Used extensively as a helper for monadic operations on std::expected and std::optional Contrained 'on
// the inside' so it shouldn't be possible to misuse
template <typename F, typename... Ts>
constexpr inline auto bind_back(F &&fn, Ts &&...ts) {
    return [&](auto &&...firstArgs) -> auto
           requires std::is_invocable_v<F, decltype(firstArgs)..., Ts...>
    { return std::invoke(fn, std::forward<decltype(firstArgs)>(firstArgs)..., std::forward<decltype(ts)>(ts)...); };
};


constexpr inline std::string convert_u32u8(std::u32string &str) {
    using namespace ww898::utf;
    std::string res;
    convz<utf_selector_t<std::decay_t<decltype(str)>::value_type>, utf8>(str.data(), std::back_inserter(res));
    return res;
}
constexpr inline std::string convert_u32u8(std::u32string &&str) {
    return convert_u32u8(str);
}
constexpr inline std::u32string convert_u32u8(std::string const &str) {
    using namespace ww898::utf;
    std::u32string res;
    convz<utf_selector_t<std::decay_t<decltype(str)>::value_type>, utf32>(str.data(), std::back_inserter(res));
    return res;
}
constexpr inline std::u32string convert_u32u8(std::string const &&str) {
    return convert_u32u8(str);
}

template <typename T>
constexpr inline auto get_sortedAndUniqued(T &cont) {
    auto contCpy = std::ranges::to<std::vector>(cont);
    std::ranges::sort(contCpy, std::less());
    auto [beg, end] = std::ranges::unique(contCpy);
    contCpy.erase(beg, end);
    return contCpy;
}

// Compute 'on display' size of a string (correctly taking into account UTF8 glyphs)
constexpr inline std::size_t strlen_utf8(const std::string &str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { ++length; }
    }
    return length;
}

constexpr inline std::string trim2Size_leading(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function

    if (str.size() <= maxSize) { return std::string(maxSize - strlen_utf8(str), Config::space).append(str); }
    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }
        std::string res(str.begin(), str.begin() + std::min(str.size(), 1uz));
        while (res.size() < maxSize) { res.push_back('.'); }
        return res;
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
}
constexpr inline std::string trim2Size_leading(std::string const &&str, size_t maxSize) {
    return trim2Size_leading(str, maxSize);
}
constexpr inline std::string trim2Size_ending(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function

    if (str.size() <= maxSize) {
        return std::string(str).append(std::string(maxSize - strlen_utf8(str), Config::space));
    }
    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }
        std::string res(str.begin(), str.begin() + std::min(str.size(), 1uz));
        while (res.size() < maxSize) { res.push_back('.'); }
        return res;
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
}
constexpr inline std::string trim2Size_ending(std::string const &&str, size_t maxSize) {
    return trim2Size_ending(str, maxSize);
}
constexpr inline std::string trim2Size_leadingEnding(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function

    if (str.size() <= maxSize) {
        return std::string((maxSize - strlen_utf8(str)) / 2, Config::space)
            .append(str)
            .append(
                std::string(((maxSize - strlen_utf8(str)) / 2) + ((maxSize - strlen_utf8(str)) % 2), Config::space));
    }
    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }
        std::string res(str.begin(), str.begin() + std::min(str.size(), 1uz));
        while (res.size() < maxSize) { res.push_back('.'); }
        return res;
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
}
constexpr inline std::string trim2Size_leadingEnding(std::string const &&str, size_t maxSize) {
    return trim2Size_leadingEnding(str, maxSize);
}

constexpr size_t get_axisFillerSize(size_t axisLength, size_t axisStepCount) {
    return (axisLength - axisStepCount) / (axisStepCount + 1);
}

constexpr inline std::vector<std::string> create_tickMarkedAxis(std::string filler, std::string tick, size_t steps,
                                                                size_t totalLength) {
    size_t fillerSize = get_axisFillerSize(totalLength, steps);

    std::vector<std::string> res;
    for (size_t i_step = 0; i_step < steps; ++i_step) {
        for (size_t i_filler = 0; i_filler < fillerSize; ++i_filler) {
            res.push_back(TermColors::get_coloured(filler, Config::color_Axes_enum));
        }
        res.push_back(TermColors::get_coloured(tick, Config::color_Axes_enum));
    }
    size_t sizeOfRest = totalLength - (steps) - (steps * fillerSize);
    for (size_t i_filler = 0; i_filler < sizeOfRest; ++i_filler) {
        res.push_back(TermColors::get_coloured(filler, Config::color_Axes_enum));
    }
    return res;
}
constexpr inline size_t guess_stepsOnHorAxis(long long width, size_t maxLabelSize = Config::max_valLabelSize) {
    // Subtract the beginning and the end label sizes and -2 for spacing
    width += (-2 * maxLabelSize + 2) - 2;
    return (std::max(0ll, width) / (maxLabelSize + 4));
}
constexpr inline size_t guess_stepsOnVerAxis(long long height, size_t verticalStepSize = Config::axis_stepSize_vl) {
    // Subtract the beginning and the end label sizes and -2 for spacing
    height -= (verticalStepSize - 1);
    return (std::max(0ll, height) / verticalStepSize);
}

template <typename T>
requires std::is_arithmetic_v<std::decay_t<T>>
constexpr inline std::pair<double, std::string> rebase_2_SIPrefix(T &&value) {
    if (value == 0) { return {0, ""}; }
    else {
        int target = std::log10(std::abs(value)) / 3 - (std::abs(value) < 1 ? 1 : 0);
        return {value / std::pow(1000, target), Config::si_prefixes.at(target + 10)};
    }
}

template <typename T>
requires std::is_arithmetic_v<std::decay_t<T>>
constexpr inline std::string format_toMax5length(T &&val) {
    auto [rbsed, unit] = rebase_2_SIPrefix(std::forward<decltype(val)>(val));
    return std::format("{:.{}f}{}", rbsed, (rbsed >= 10 || rbsed <= -10) ? 0 : 1, unit);
}

using variadicColumns = std::variant<std::pair<std::string, std::reference_wrapper<const std::vector<long long>>>,
                                     std::pair<std::string, std::reference_wrapper<const std::vector<double>>>>;

constexpr inline std::tuple<double, double> compute_minMaxMulti(auto &&vectorOfVariantViews) {
    std::pair<double, double> res{std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};

    auto ol_set = [&](auto &var) -> void {
        using val_type = std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>;
        if constexpr (std::is_arithmetic_v<val_type>) {

            auto [minV_l, maxV_l] = std::ranges::minmax(var);
            res.first             = std::min(res.first, static_cast<double>(minV_l));
            res.second            = std::max(res.second, static_cast<double>(maxV_l));
        }
    };
    if constexpr (std::ranges::range<decltype(vectorOfVariantViews)>) {
        for (auto &variantRef : vectorOfVariantViews) { std::visit(ol_set, variantRef); }
    }
    else { std::visit(ol_set, vectorOfVariantViews); }
    return res;
}


} // namespace detail
} // namespace terminal_plot
} // namespace incom