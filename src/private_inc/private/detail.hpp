#pragma once

#include <algorithm>
#include <cmath>
#include <format>
#include <functional>
#include <iterator>
#include <ranges>
#include <string_view>
#include <type_traits>

#include <private/color_mixer.hpp>
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
constexpr inline std::string convert_u32u8(std::u32string_view &str_v) {
    using namespace ww898::utf;
    std::string res;
    convz<utf_selector_t<std::decay_t<decltype(str_v)>::value_type>, utf8>(str_v.data(), std::back_inserter(res));
    return res;
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
constexpr inline std::u32string convert_u32u8(std::string_view const str_v) {
    using namespace ww898::utf;
    std::u32string res;
    convz<utf_selector_t<std::decay_t<decltype(str_v)>::value_type>, utf32>(str_v.data(), std::back_inserter(res));
    return res;
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

constexpr inline std::size_t conv_utf8CharCount_to_charCount(const std::string &str, size_t utf8CharCount) {
    std::size_t res = 0;
    utf8CharCount++;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { utf8CharCount--; }
        if (utf8CharCount == 0) { return res; }
        ++res;
    }
    return res;
}

constexpr inline std::string trim2Size_leading(std::string const &str, size_t maxSize) {
    long long const strLen = strlen_utf8(str);
    if (strLen <= maxSize) { return std::string(maxSize - strLen, Config::space).append(str); }

    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }
        std::string res(std::max(0ll, static_cast<long long>(maxSize) - strLen), Config::space);

        size_t addFromInput = maxSize - res.size() - 1 + (maxSize >= strLen);
        res.append(std::string(str.begin(), str.begin() + conv_utf8CharCount_to_charCount(str, addFromInput)));

        // Elipsis is added only if str was shortened
        if (maxSize < strLen) { res.append(Config::elipsis); }
        return res;
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        size_t cp       = conv_utf8CharCount_to_charCount(str, cutPoint);
        size_t cpNext   = conv_utf8CharCount_to_charCount(str, cutPoint + 1 + (strLen - maxSize));

        return std::string(str.begin(), str.begin() + cp)
            .append(Config::elipsis)
            .append(str.begin() + cpNext, str.end());
    }
}
constexpr inline std::string trim2Size_leading(std::string &&str, size_t maxSize) {
    return trim2Size_leading(str, maxSize);
}
constexpr inline std::string trim2Size_ending(std::string const &str, size_t maxSize) {
    long long const strLen = strlen_utf8(str);
    if (strLen <= maxSize) { return std::string(str).append(std::string(maxSize - strLen, Config::space)); }
    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }

        std::string res(str.begin(),
                        str.begin() + conv_utf8CharCount_to_charCount(str, maxSize - 1 + (maxSize >= strLen)));

        // Elipsis is added only if str was shortened
        if (maxSize < strLen) { res.append(Config::elipsis); }
        else {
            long long addSpaces = static_cast<long long>(maxSize) - strLen;
            while (addSpaces-- > 0) { res.push_back(Config::space); }
        }
        return res;
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        size_t cp       = conv_utf8CharCount_to_charCount(str, cutPoint);
        size_t cpNext   = conv_utf8CharCount_to_charCount(str, cutPoint + 1 + (strLen - maxSize));

        return std::string(str.begin(), str.begin() + cp)
            .append(Config::elipsis)
            .append(str.begin() + cpNext, str.end());
    }
}
constexpr inline std::string trim2Size_ending(std::string &&str, size_t maxSize) {
    return trim2Size_ending(str, maxSize);
}
constexpr inline std::string trim2Size_leadingEnding(std::string const &str, size_t maxSize) {
    long long const strLen = strlen_utf8(str);
    if (strLen <= maxSize) {
        return std::string((maxSize - strLen) / 2, Config::space)
            .append(str)
            .append(std::string(maxSize - strLen - ((maxSize - strLen) / 2), Config::space));
    }
    // Special cases where maxSize is very small
    else if (maxSize < 5) {
        if (maxSize == 0) { return ""; }
        else {
            std::string res(str.begin(), str.begin() + conv_utf8CharCount_to_charCount(str, maxSize - 1));

            // Elipsis is added only if str was shortened
            res.append(Config::elipsis);
            return res;
        }
    }
    // Regular case, we know that size of string is larger than maxSize
    else {
        size_t cutPoint = maxSize / 2;
        size_t cp       = conv_utf8CharCount_to_charCount(str, cutPoint);
        size_t cpNext   = conv_utf8CharCount_to_charCount(str, cutPoint + 1 + (strLen - maxSize));

        return std::string(str.begin(), str.begin() + cp)
            .append(Config::elipsis)
            .append(str.begin() + cpNext, str.end());
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
            res.push_back(ANSI::SGR_builder().color_fg(Config::color_axesDefault).add_string(filler).reset_all().get());
        }
        res.push_back(ANSI::SGR_builder().color_fg(Config::color_axesDefault).add_string(tick).reset_all().get());
    }
    size_t sizeOfRest = totalLength - (steps) - (steps * fillerSize);
    for (size_t i_filler = 0; i_filler < sizeOfRest; ++i_filler) {
        res.push_back(ANSI::SGR_builder().color_fg(Config::color_axesDefault).add_string(filler).reset_all().get());
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
requires std::is_arithmetic_v<std::remove_cvref_t<T>>
constexpr inline std::pair<double, std::string> rebase_2_SIPrefix(T &&value) {
    if (value == 0) { return {0, ""}; }
    else {
        int target = std::log10(std::abs(value));
        target     = (target - (3 * (target == 3))) / 3 - (std::abs(value) < 1 ? 1 : 0);
        return {value / std::pow(1000, target), Config::si_prefixes.at(target + 10)};
    }
}

template <typename T>
requires std::is_arithmetic_v<std::remove_cvref_t<T>>
constexpr inline std::string format_toMax5length(T &&val) {
    auto [rbsed, unit] = rebase_2_SIPrefix(std::forward<decltype(val)>(val));
    return std::format("{:.{}f}{}", rbsed, (rbsed >= 10 || rbsed <= -10) ? 0 : 1, unit);
}

} // namespace detail
} // namespace terminal_plot
} // namespace incom