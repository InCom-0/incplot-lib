#pragma once

#include "incplot/config.hpp"
#include "incplot/detail/color.hpp"
#include <cmath>
#include <codecvt>
#include <format>
#include <functional>
#include <locale>

#include <incplot/color_mixer.hpp>
#include <incplot/detail/concepts.hpp>
#include <incplot/detail/misc.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>


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

constexpr std::string trim2Size_leading(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function
    if (str.size() > maxSize) {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
    else { return std::string(maxSize - strlen_utf8(str), Config::space).append(str); }
}
constexpr std::string trim2Size_leading(std::string const &&str, size_t maxSize) {
    return trim2Size_leading(str, maxSize);
}
constexpr std::string trim2Size_leadingEnding(std::string const &str, size_t maxSize) {
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
constexpr std::string trim2Size_leadingEnding(std::string const &&str, size_t maxSize) {
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

class BrailleDrawer {
private:
    std::vector<std::vector<std::u32string>> m_canvasColors;
    std::vector<std::vector<char32_t>>       m_canvasBraille;
    std::vector<std::u32string>              m_colorPallete = std::vector<std::u32string>{
        detail::convert_u32u8(Config::color_Vals1), detail::convert_u32u8(Config::color_Vals2),
        detail::convert_u32u8(Config::color_Vals3)};
    std::u32string s_terminalDefault = detail::convert_u32u8(Config::term_setDefault);


    BrailleDrawer() {};
    BrailleDrawer(size_t canvas_width, size_t canvas_height)
        : m_canvasColors(std::vector(canvas_height, std::vector<std::u32string>(canvas_width, U""))),
          m_canvasBraille(std::vector(canvas_height, std::vector<char32_t>(canvas_width, Config::braille_blank))) {};

    constexpr void compute_canvasColors(
        std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> const &points_perDotPerColor) {
        ColorMixer cm(ColorMixer::compute_maxStepsPerColor(points_perDotPerColor));
        for (size_t rowID = 0; rowID < points_perDotPerColor.size(); ++rowID) {
            for (size_t colID = 0; colID < points_perDotPerColor[rowID].size(); ++colID) {
                if (m_canvasBraille[rowID][colID] != Config::braille_blank) {
                    m_canvasColors[rowID][colID] = detail::convert_u32u8(
                        TermColors::get_fgColor(cm.compute_colorOfPosition(points_perDotPerColor[rowID][colID])));
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
    using variadicColumns = std::variant<std::pair<std::string, std::reference_wrapper<const std::vector<long long>>>,
                                         std::pair<std::string, std::reference_wrapper<const std::vector<double>>>>;

    template <typename Y, typename X>
    requires std::is_arithmetic_v<typename X::value_type> && std::is_arithmetic_v<typename Y::value_type>
    static constexpr std::vector<std::string> drawPoints(
        size_t canvas_width, size_t canvas_height, Y const &y_values, X const &x_values,
        std::optional<std::variant<std::reference_wrapper<std::vector<std::string>>,
                                   std::reference_wrapper<std::vector<long long>>,
                                   std::reference_wrapper<std::vector<double>>>> const catCol = std::nullopt) {


        size_t                   numOf_categories;
        std::vector<std::string> catNames;
        std::vector<size_t>      catCol_IDs = {};
        if (catCol.has_value()) {
            // Computes: 1) Vector of category names, 2) Vector of category IDs same size as data, same num of unique
            // IDs as in vector of category names
            auto olSet2 = [&](auto &&rng) -> void {
                auto sortedUniqued = get_sortedAndUniqued(rng.get());
                for (auto const &oneItem : sortedUniqued) {
                    if constexpr (std::is_same_v<std::string,
                                                 std::decay_t<typename decltype(sortedUniqued)::value_type>>) {
                        catNames.push_back(oneItem);
                    }
                    else { catNames.push_back(std::to_string(oneItem)); }
                }

                for (auto const &oneOrigItem : rng.get()) {
                    auto it = std::ranges::find(sortedUniqued, oneOrigItem);
                    catCol_IDs.push_back(it - sortedUniqued.begin());
                };
            };

            std::visit(olSet2, catCol.value());
            numOf_categories = catNames.size();
        }
        else {
            // No category column means we only have one category in this drawPoint overload
            // catCol_IDs in that case is just a vector filled with value 0 which is the ID of the only category
            numOf_categories = 1;
            catCol_IDs       = std::vector<size_t>(x_values.size(), 0);
        }

        std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> pointsCountPerPos_perColor =
            (std::vector(
                canvas_height,
                std::vector(canvas_width,
                            std::array<std::array<std::vector<size_t>, 2>, 4>{
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0)})));


        BrailleDrawer bd(canvas_width, canvas_height);

        auto [yMin, yMax] = std::ranges::minmax(y_values);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);

        std::vector<double> xMinCol;
        std::vector<double> xMaxCol;
        std::vector<double> xstepSizeCol;

        auto compute_minMaxStepSize = [&](auto &&fv) -> void {
            auto [xMin, xMax] = std::ranges::minmax(fv);
            xstepSizeCol.push_back((xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1));
            xMinCol.push_back(xMin);
            xMaxCol.push_back(xMax);
        };

        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            auto y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            auto yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            auto x       = static_cast<size_t>(((xVal - xMinCol[groupID]) / xstepSizeCol[groupID])) / 2;
            auto xChrPos = static_cast<size_t>(((xVal - xMinCol[groupID]) / xstepSizeCol[groupID])) % 2;

            bd.m_canvasBraille[y][x] |= Config::braille_map[yChrPos][xChrPos];
            pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]++;
        };


        // Compute min, max, stepSize for every xVal column there is. If only one (ie. without 'catCol') that's fine
        for (size_t i = 0; i < numOf_categories; ++i) {
            auto ff = std::views::zip(x_values, catCol_IDs) |
                      std::views::filter([&](auto &&tup) { return (std::get<1>(tup) == i); }) |
                      std::views::transform([&](auto &&a) { return std::get<0>(a); });
            compute_minMaxStepSize(ff);
        }

        for (size_t i = 0; i < y_values.size(); ++i) { placePointOnCanvas(y_values[i], x_values[i], catCol_IDs[i]); }
        bd.compute_canvasColors(pointsCountPerPos_perColor);
        return bd.construct_outputPlotArea();
    }
    template <typename Y>
    requires std::is_arithmetic_v<typename Y::value_type>
    static constexpr std::vector<std::string> drawPoints(
        size_t canvas_width, size_t canvas_height, Y const &y_values,
        std::vector<std::variant<std::pair<std::string, std::reference_wrapper<const std::vector<long long>>>,
                                 std::pair<std::string, std::reference_wrapper<const std::vector<double>>>>> const
            &x_valCols) {


        size_t                                                                      numOf_categories = x_valCols.size();
        std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> pointsCountPerPos_perColor =
            (std::vector(
                canvas_height,
                std::vector(canvas_width,
                            std::array<std::array<std::vector<size_t>, 2>, 4>{
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0),
                                std::vector<size_t>(numOf_categories, 0), std::vector<size_t>(numOf_categories, 0)})));


        BrailleDrawer bd(canvas_width, canvas_height);

        auto [yMin, yMax] = std::ranges::minmax(y_values);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);

        std::vector<double> xMinCol;
        std::vector<double> xMaxCol;
        std::vector<double> xstepSizeCol;

        auto compute_minMaxStepSize = [&](auto &&fv) -> void {
            auto [xMin, xMax] = std::ranges::minmax(fv);
            xstepSizeCol.push_back((xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1));
            xMinCol.push_back(xMin);
            xMaxCol.push_back(xMax);
        };

        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            auto y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            auto yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            auto x       = static_cast<size_t>(((xVal - xMinCol[groupID]) / xstepSizeCol[groupID])) / 2;
            auto xChrPos = static_cast<size_t>(((xVal - xMinCol[groupID]) / xstepSizeCol[groupID])) % 2;

            bd.m_canvasBraille[y][x] |= Config::braille_map[yChrPos][xChrPos];
            pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]++;
        };

        // Category is specified by individual x_valCols themselves (their heading is the category name)
        for (size_t i = 0; auto const &one_xValCol : x_valCols) {
            std::visit([&](auto const &pair) -> void { compute_minMaxStepSize(pair.second.get()); }, one_xValCol);
        }
        for (size_t i = 0; auto const &one_xValCol : x_valCols) {
            auto olSet = [&](auto const &pair) -> void {
                auto const &xValCol_data = pair.second.get();
                for (size_t rowID = 0; rowID < y_values.size(); ++rowID) {
                    placePointOnCanvas(y_values[rowID], xValCol_data[rowID], i);
                }
                i++;
            };
            std::visit(olSet, one_xValCol);
        }

        bd.compute_canvasColors(pointsCountPerPos_perColor);
        return bd.construct_outputPlotArea();
    }
};

} // namespace detail
} // namespace terminal_plot
} // namespace incom