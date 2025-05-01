#pragma once

#include <cmath>
#include <cuchar>
#include <format>

#include <incplot/color_mixer.hpp>
#include <incplot/detail/concepts.hpp>
#include <incplot/detail/misc.hpp>


namespace incom {
namespace terminal_plot {
namespace detail {
constexpr inline std::string convert_u32u8(std::u32string const &str) {
    std::mbstate_t mbstate_{};
    std::string    result;
    size_t         mblen;
    char           mbchar[MB_CUR_MAX];

    for (char32_t const wc : str) {
        mblen = std::c32rtomb(mbchar, wc, &mbstate_);
        if (mblen == static_cast<size_t>(-1)) { throw std::runtime_error("Invalid wide character encountered."); }
        result.append(mbchar, mblen);
    }
    return result;
}
constexpr inline std::string convert_u32u8(std::u32string const &&str) {
    return convert_u32u8(str);
}
constexpr inline std::u32string convert_u32u8(std::string const &str) {
    std::mbstate_t mbstate_{};
    std::u32string result;
    size_t         mblen;
    char32_t       wc = U'\0';
    auto const endOut = str.end();

    for (auto it = str.begin(); it < endOut;) {
        if (mblen == static_cast<size_t>(-1)) { throw std::runtime_error("Invalid multibyte sequence encountered."); }
        else if (mblen == static_cast<size_t>(-2)) {
            throw std::runtime_error("Incomplete multibyte sequence encountered.");
        }
        else if (mblen == 0) {
            break; // Null character encountered, terminate the loop.
        }

        mblen   = std::mbrtoc32(&wc, &(*it), endOut - it, &mbstate_);
        result += wc;
        it     += mblen;
    }
    return result;
}
constexpr inline std::u32string convert_u32u8(std::string const &&str) {
    return convert_u32u8(str);
}

template <typename T>
constexpr inline auto get_sortedAndUniqued(T &cont) {
    auto contCpy = cont;
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

// TODO: Also make a version where the tick positions are explictily specified in one vector of size_t
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
    // Substract the beginning and the end label sizes and -2 for spacing
    width += (-2 * maxLabelSize + 2) - 2;
    return (std::max(0ll, width) / (maxLabelSize + 4));
}
constexpr inline size_t guess_stepsOnVerAxis(long long height, size_t verticalStepSize = Config::axis_stepSize_vl) {
    // Substract the beginning and the end label sizes and -2 for spacing
    height -= (verticalStepSize - 1);
    return (std::max(0ll, height) / verticalStepSize);
}


// Ring vector for future usage in 'scrolling plot' scenarios
template <typename T>
class RingVector {
private:
    std::vector<T> _m_buf;
    size_t         head        = 0;
    size_t         nextRead_ID = 0;

public:
    RingVector(std::vector<T> &&t) : _m_buf(t) {};
    RingVector(std::vector<T> &t) : _m_buf(t) {};

    // TODO: Might not need to create a copy here or below once std::views::concatenate from C++26 exists
    std::vector<T> create_copy() {
        std::vector<T> res(_m_buf.begin() + head, _m_buf.end());
        for (int i = 0; i < head; ++i) { res.push_back(_m_buf[i]); }
        return res;
    }

    std::vector<T> create_copy_reversed() {
        std::vector<T> res(_m_buf.rbegin() + (_m_buf.size() - head), _m_buf.rend());
        for (int i = (_m_buf.size() - 1); i >= head; --i) { res.push_back(_m_buf[i]); }
        return res;
    }

    T get_cur() const { return _m_buf[nextRead_ID]; }
    T get_cur_and_next() {
        T res = get_cur();
        advanceByOne();
        return res;
    }
    T get_cur_and_advanceBy(size_t by = 1) {
        T res = get_cur();
        advanceBy(by);
        return res;
    }

    void inline advanceByOne() { nextRead_ID = (nextRead_ID + 1) % _m_buf.size(); }
    void inline advanceBy(int by = 1) { nextRead_ID = (nextRead_ID + by) % _m_buf.size(); }

    // On insertion resets nextRead_ID to head as well
    void insertAtHead(T &&item) { insertAtHead(item); }
    void insertAtHead(T const &item) {
        _m_buf[head] = item;
        head         = (head + 1) % _m_buf.size();
        nextRead_ID  = head;
    }
};

template <typename T>
requires std::is_arithmetic_v<std::decay_t<T>>
constexpr inline std::pair<double, std::optional<std::string>> rebase_2_SIPrefix(T &&value) {
    if (value == 0) { return {0, ""}; }
    else {
        T   absVal = value < 0 ? (-value) : value;
        int target = absVal >= 1 ? (std::log10(absVal) / 3) : (std::log10(absVal) / 3) - 1;
        return {value / std::pow(1000, target), Config::si_prefixes.at(target + 10)};
    }
}

template <typename T>
requires std::is_arithmetic_v<std::decay_t<T>>
constexpr inline std::string format_toMax5length(T &&val) {
    auto [rbsed, unit] = rebase_2_SIPrefix(std::forward<decltype(val)>(val));
    return std::format("{:.{}f}{}", rbsed, (rbsed >= 10 || rbsed <= -10) ? 0 : 1, unit.value_or(""));
}

using variadicColumns = std::variant<std::pair<std::string, std::reference_wrapper<const std::vector<long long>>>,
                                     std::pair<std::string, std::reference_wrapper<const std::vector<double>>>>;

constexpr inline std::tuple<double, double> compute_minMaxMulti(auto &&vectorOfVariantRefWrpVectors) {
    std::pair<double, double> res{std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};

    auto ol_set = [&](auto &var) -> void {
        auto const &vect = var.get();
        if constexpr (std::is_arithmetic_v<typename std::remove_reference_t<decltype(vect)>::value_type>) {
            auto [minV_l, maxV_l] = std::ranges::minmax(vect);
            res.first             = std::min(res.first, static_cast<double>(minV_l));
            res.second            = std::max(res.second, static_cast<double>(maxV_l));
        }
    };

    for (auto const &variantRef : vectorOfVariantRefWrpVectors) { std::visit(ol_set, variantRef); }
    return res;
}


} // namespace detail
} // namespace terminal_plot
} // namespace incom