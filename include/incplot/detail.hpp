#pragma once

#include "incplot/config.hpp"
#include "incplot/detail/color.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <codecvt>
#include <locale>

#include <incplot/color_mixer.hpp>
#include <incplot/desired_plot.hpp>
#include <incplot/detail/concepts.hpp>
#include <incplot/detail/misc.hpp>
#include <string>


namespace incom {
namespace terminal_plot {
namespace detail {
constexpr inline std::string convert_u32u8(std::u32string const &str) {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(str);
}
constexpr inline std::string convert_u32u8(std::u32string const &&str) {
    return convert_u32u8(str);
}
constexpr inline std::u32string convert_u32u8(std::string const &str) {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(str);
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
    if (str.size() > maxSize) {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
    else { return std::string(maxSize - strlen_utf8(str), Config::space).append(str); }
}
constexpr inline std::string trim2Size_leading(std::string const &&str, size_t maxSize) {
    return trim2Size_leading(str, maxSize);
}
constexpr inline std::string trim2Size_ending(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function
    if (str.size() > maxSize) {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
    else { return std::string(str).append(std::string(maxSize - strlen_utf8(str), Config::space)); }
}
constexpr inline std::string trim2Size_ending(std::string const &&str, size_t maxSize) {
    return trim2Size_ending(str, maxSize);
}
constexpr inline std::string trim2Size_leadingEnding(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function
    if (str.size() > maxSize) {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
    else {
        return std::string((maxSize - strlen_utf8(str)) / 2, Config::space)
            .append(str)
            .append(
                std::string(((maxSize - strlen_utf8(str)) / 2) + ((maxSize - strlen_utf8(str)) % 2), Config::space));
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
constexpr inline size_t guess_stepsOnHorAxis(size_t width, size_t maxLabelSize = Config::max_valLabelSize) {
    // Substract the beginning and the end label sizes and -2 for spacing
    width += (-2 * maxLabelSize + 2) - 2;
    return (width / (maxLabelSize + 4));
}
constexpr inline size_t guess_stepsOnVerAxis(size_t height, size_t verticalStepSize = Config::axis_stepSize_vl) {
    // Substract the beginning and the end label sizes and -2 for spacing
    height += -(verticalStepSize - 1);
    return (height / verticalStepSize);
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

constexpr inline std::pair<double, double> compute_minMaxMulti(auto &&vectorOfVariantRefWrpVectors) {
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


class BrailleDrawer {
private:
    std::vector<std::vector<std::u32string>>                                    m_canvasColors;
    std::vector<std::vector<char32_t>>                                          m_canvasBraille;
    std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> m_pointsCountPerPos_perColor;

    std::array<std::u32string, 6> m_colorPallete;
    std::u32string                s_terminalDefault = detail::convert_u32u8(Config::term_setDefault);

    BrailleDrawer() {};
    BrailleDrawer(size_t canvas_width, size_t canvas_height, size_t numOf_categories,
                  std::array<Color_CVTS, 6> const &colorPalette)
        : m_canvasColors(std::vector(canvas_height, std::vector<std::u32string>(canvas_width, U""))),
          m_canvasBraille(std::vector(canvas_height, std::vector<char32_t>(canvas_width, Config::braille_blank))),
          m_colorPallete{detail::convert_u32u8(TermColors::get_basicColor(colorPalette[0])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[1])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[2])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[3])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[4])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[5]))},
          m_pointsCountPerPos_perColor(std::vector(
              canvas_height,
              std::vector(canvas_width,
                          std::array<std::array<std::vector<size_t>, 2>, 4>{
                              std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                              std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                              std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                              std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0)}))) {};

    constexpr void compute_canvasColors() {
        ColorMixer cm(ColorMixer::compute_maxStepsPerColor(m_pointsCountPerPos_perColor));
        for (size_t rowID = 0; rowID < m_pointsCountPerPos_perColor.size(); ++rowID) {
            for (size_t colID = 0; colID < m_pointsCountPerPos_perColor[rowID].size(); ++colID) {
                if (m_canvasBraille[rowID][colID] != Config::braille_blank) {
                    m_canvasColors[rowID][colID] = detail::convert_u32u8(TermColors::get_fgColor(
                        cm.compute_colorOfPosition(m_pointsCountPerPos_perColor[rowID][colID])));
                }
            }
        }
    }
    constexpr std::vector<std::string> construct_outputPlotArea() {
        std::vector<std::string> res;
        // Gotta start rows from the back because axes cross bottom left and 'row 0' is top left
        for (int rowID = m_canvasBraille.size() - 1; rowID > -1; --rowID) {
            std::u32string oneLine;
            for (size_t colID = 0; colID < m_canvasBraille.front().size(); ++colID) {
                if (m_canvasColors[rowID][colID].empty()) { oneLine.push_back(m_canvasBraille[rowID][colID]); }
                else {
                    oneLine.append(m_canvasColors[rowID][colID]);
                    oneLine.push_back(m_canvasBraille[rowID][colID]);
                    oneLine.append(s_terminalDefault);
                }
            }
            res.push_back(detail::convert_u32u8(oneLine));
        }
        return res;
    }

public:
    template <typename X>
    requires std::is_arithmetic_v<typename X::value_type>
    static constexpr std::vector<std::string> drawPoints(size_t canvas_width, size_t canvas_height, X const &x_values,
                                                         auto                                      viewOfValVariants,
                                                         std::optional<std::vector<size_t>> const &catIDs_vec,
                                                         std::array<Color_CVTS, 6>                 colorPalette) {

        BrailleDrawer bd(canvas_width, canvas_height,
                         catIDs_vec.has_value()
                             ? get_sortedAndUniqued(catIDs_vec.value()).size()
                             : std::ranges::count_if(viewOfValVariants, [](auto const &a) { return true; }),
                         colorPalette);

        auto [xMin, xMax] = std::ranges::minmax(x_values);
        auto [yMin, yMax] = compute_minMaxMulti(viewOfValVariants);
        double xStepSize  = (xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);


        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            auto y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            auto yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            auto x       = static_cast<size_t>(((xVal - xMin) / xStepSize)) / 2;
            auto xChrPos = static_cast<size_t>(((xVal - xMin) / xStepSize)) % 2;

            bd.m_canvasBraille[y][x] |= Config::braille_map[yChrPos][xChrPos];
            bd.m_pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]++;
        };

        for (size_t i = 0; auto const &one_yValCol : viewOfValVariants) {
            auto olSet = [&](auto const &oneCol) -> void {
                auto const &yValCol_data = oneCol.get();
                if constexpr (std::is_arithmetic_v<
                                  typename std::remove_reference_t<decltype(yValCol_data)>::value_type>) {
                    if (catIDs_vec.has_value()) {
                        for (size_t rowID = 0; rowID < x_values.size(); ++rowID) {
                            placePointOnCanvas(yValCol_data[rowID], x_values[rowID], catIDs_vec.value()[rowID]);
                        }
                    }
                    else {
                        for (size_t rowID = 0; rowID < x_values.size(); ++rowID) {
                            placePointOnCanvas(yValCol_data[rowID], x_values[rowID], i);
                        }
                    }
                    i++;
                }
            };
            std::visit(olSet, one_yValCol);
        }

        bd.compute_canvasColors();
        return bd.construct_outputPlotArea();
    }
};


} // namespace detail
} // namespace terminal_plot
} // namespace incom