#pragma once

#include <algorithm>
#include <cmath>
#include <expected>
#include <optional>
#include <print>
#include <ranges>
#include <source_location>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include <nlohmann/json.hpp>
#include <vector>


namespace incom {
namespace terminal_plot {

// FORWARD DELCARATIONS
namespace plot_structures {
class Base;
class BarV;
class BarH;
class Line;
class Multiline;
class Scatter;
class Bubble;
} // namespace plot_structures

struct DataStore;
struct Parser;
class DesiredPlot;

// FORWARD DELCARATIONS --- END

enum class Color_CVTS {
    Default                   = 0,
    Bold_or_Bright            = 1,
    No_bold_or_bright         = 22,
    Underline                 = 4,
    No_underline              = 24,
    Negative                  = 7,
    Positive_No_negative      = 27,
    Foreground_Black          = 30,
    Foreground_Red            = 31,
    Foreground_Green          = 32,
    Foreground_Yellow         = 33,
    Foreground_Blue           = 34,
    Foreground_Magenta        = 35,
    Foreground_Cyan           = 36,
    Foreground_White          = 37,
    Foreground_Extended       = 38,
    Foreground_Default        = 39,
    Background_Black          = 40,
    Background_Red            = 41,
    Background_Green          = 42,
    Background_Yellow         = 43,
    Background_Blue           = 44,
    Background_Magenta        = 45,
    Background_Cyan           = 46,
    Background_White          = 47,
    Background_Extended       = 48,
    Background_Default        = 49,
    Bright_Foreground_Black   = 90,
    Bright_Foreground_Red     = 91,
    Bright_Foreground_Green   = 92,
    Bright_Foreground_Yellow  = 93,
    Bright_Foreground_Blue    = 94,
    Bright_Foreground_Magenta = 95,
    Bright_Foreground_Cyan    = 96,
    Bright_Foreground_White   = 97,
    Bright_Background_Black   = 100,
    Bright_Background_Red     = 101,
    Bright_Background_Green   = 102,
    Bright_Background_Yellow  = 103,
    Bright_Background_Blue    = 104,
    Bright_Background_Magenta = 105,
    Bright_Background_Cyan    = 106,
    Bright_Background_White   = 107
};

class TermColors {
private:
    static constexpr auto const _S_basicCVTScolMap = std::array<std::pair<int, std::string_view>, 43>{{
        {0, "\x1b[m"},      {1, "\x1b[1m"},     {22, "\x1b[22m"},   {4, "\x1b[4m"},     {24, "\x1b[24m"},
        {7, "\x1b[7m"},     {27, "\x1b[27m"},   {30, "\x1b[30m"},   {31, "\x1b[31m"},   {32, "\x1b[32m"},
        {33, "\x1b[33m"},   {34, "\x1b[34m"},   {35, "\x1b[35m"},   {36, "\x1b[36m"},   {37, "\x1b[37m"},
        {38, "\x1b[38m"},   {39, "\x1b[39m"},   {40, "\x1b[40m"},   {41, "\x1b[41m"},   {42, "\x1b[42m"},
        {43, "\x1b[43m"},   {44, "\x1b[44m"},   {45, "\x1b[45m"},   {46, "\x1b[46m"},   {47, "\x1b[47m"},
        {48, "\x1b[48m"},   {49, "\x1b[49m"},   {90, "\x1b[90m"},   {91, "\x1b[91m"},   {92, "\x1b[92m"},
        {93, "\x1b[93m"},   {94, "\x1b[94m"},   {95, "\x1b[95m"},   {96, "\x1b[96m"},   {97, "\x1b[97m"},
        {100, "\x1b[100m"}, {101, "\x1b[101m"}, {102, "\x1b[102m"}, {103, "\x1b[103m"}, {104, "\x1b[104m"},
        {105, "\x1b[105m"}, {106, "\x1b[106m"}, {107, "\x1b[107m"},
    }};

    static constexpr auto const _S_extendedCVTScolMap = std::array<std::pair<int, std::string_view>, 43>{{{1, "2"}}};

public:
    // Class should be impossible to instantiate
    TermColors()                    = delete;
    void *operator new(std::size_t) = delete;

    static constexpr auto get_basicColor(Color_CVTS const col) {
        return std::string(std::ranges::find_if(_S_basicCVTScolMap, [&](auto &&pr) {
                               return pr.first == static_cast<int>(col);
                           })->second);
    }
    static constexpr auto get_fgColor(int const color_256) {
        return std::string("\x1b[38;5;").append(std::to_string(color_256)).append("m");
    }
    static constexpr auto get_bgColor(int const color_256) {
        return std::string("\x1b[48;5;").append(std::to_string(color_256)).append("m");
    }
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_colouredFG(T &&toColor, int color_256) {
        return std::string(get_fgColor(color_256))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_colouredBG(T &&toColor, int color_256) {
        return std::string(get_bgColor(color_256))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_coloured(T &&toColor, Color_CVTS const col) {
        return std::string(get_basicColor(col))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }
};

class Config {
public:
    static constexpr std::string axisTick_l = "┤";
    static constexpr std::string axisTick_b = "┬";
    static constexpr std::string axisTick_r = "├";
    static constexpr std::string axisTick_t = "┴";

    static constexpr std::string axisFiller_l = "│";
    static constexpr std::string axisFiller_b = "─";
    static constexpr std::string axisFiller_r = "│";
    static constexpr std::string axisFiller_t = "─";

    static constexpr std::string areaCorner_tl = "┌";
    static constexpr std::string areaCorner_bl = "└";
    static constexpr std::string areaCorner_br = "┘";
    static constexpr std::string areaCorner_tr = "┐";

    static constexpr Color_CVTS color_Axes_enum  = Color_CVTS::Bright_Foreground_Black;
    static constexpr Color_CVTS color_Vals1_enum = Color_CVTS::Foreground_Green;
    static constexpr Color_CVTS color_Vals2_enum = Color_CVTS::Foreground_Red;
    static constexpr Color_CVTS color_Vals3_enum = Color_CVTS::Foreground_Blue;

    static constexpr std::string color_Axes  = TermColors::get_basicColor(color_Axes_enum);
    static constexpr std::string color_Vals1 = TermColors::get_basicColor(color_Vals1_enum);
    static constexpr std::string color_Vals2 = TermColors::get_basicColor(color_Vals2_enum);
    static constexpr std::string color_Vals3 = TermColors::get_basicColor(color_Vals3_enum);

    static constexpr size_t max_numOfValCols = 4uz;

    static constexpr size_t axisLabels_maxLength_vl = 30uz;
    static constexpr size_t axisLabels_maxLength_vr = 30uz;

    static constexpr size_t axis_stepSize_vl = 5uz;
    static constexpr size_t axis_stepSize_vr = 5uz;

    static constexpr size_t axis_verName_width_vl = 3uz;
    static constexpr size_t axis_verName_width_vr = 3uz;

    static constexpr std::string term_setDefault = TermColors::get_basicColor(Color_CVTS::Default);

    static constexpr std::array<std::string, 21> const si_prefixes{"q", "r", "y", "z", "a", "f", "p", "n", "μ", "m", "",
                                                                   "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q"};

    // 4 rows by 2 cols of braille 'single dots' for composition by 'bitwise or' into all braille chars
    static constexpr std::array<std::array<char32_t, 2>, 4> braille_map{U'⡀', U'⢀', U'⠄', U'⠠', U'⠂', U'⠐', U'⠁', U'⠈'};
    static constexpr char32_t                               braille_blank = U'⠀';

    static constexpr std::array<char32_t, 9> blocks_ver{U' ', U'▁', U'▂', U'▃', U'▄', U'▅', U'▆', U'▇', U'█'};
    static constexpr std::array<char32_t, 9> blocks_hor{U' ', U'▏', U'▎', U'▍', U'▌', U'▋', U'▊', U'▉', U'█'};
    static constexpr std::array<char32_t, 4> blocks_shades_LMD{U' ', U'░', U'▒', U'▓'};

    // TODO: Fix this so that valLabelSize is calculated from the actual possible label strings of values ... this will
    // be tremendously easier with c++26 so for the time being this is assuming maxLabelSize of 5
    static constexpr size_t max_valLabelSize = []() {
        /* std::vector<std::string> vect;

        double maxVal = std::numeric_limits<double>::max();
        double minVal = std::numeric_limits<double>::min();
        while (maxVal != 0 && minVal != 0) {
            vect.push_back(std::string("AAAAA"));
            vect.push_back(std::string("AAAAA"));
            maxVal /= 10;
            minVal /= 10;
        }

        long long maxVal_ll = std::numeric_limits<long long>::max();
        long long minVal_ll = std::numeric_limits<long long>::min();
        while (maxVal_ll != 0 && minVal_ll != 0) {
            vect.push_back(std::string("AAAAA"));
            vect.push_back(std::string("AAAAA"));
            maxVal_ll /= 10;
            minVal_ll /= 10;
        }

        size_t maxSz = 0;
        for (auto const &item : vect) { maxSz = std::max(maxSz, item.size()); }
        return maxSz; */
        return 5uz;
    }();

private:
public:
    // Class should be impossible to instantiate
    Config()                        = delete;
    void *operator new(std::size_t) = delete;
};

// UNEXPECTED AND OTHER SIMILAR ENUMS
enum class Unexp_plotSpecs {
    plotType,
    labelCol,
    valCols,
    namesIntoIDs_label,
    namesIntoIDs_vals,
    guessValCols,
    tarWidth,
    tarHeight,
    axisTicks
};
enum class Unexp_plotDrawer {
    plotStructureInvalid,
    barVplot_tooWide,
    unknownEerror
};
// UNEXPECTED AND OTHER SIMILAR ENUMS --- END

namespace detail {
constexpr inline std::string convert_u32u8(std::u32string const &str) {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(str);
}

constexpr inline std::u32string convert_u32u8(std::string const &str) {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(str);
}

// Compute 'on display' size of a string (correctly taking into account UTF8 glyphs)
constexpr inline std::size_t strlen_utf8(const std::string &str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { ++length; }
    }
    return length;
}

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

constexpr std::string trim2Size_leading(std::string const &str, size_t maxSize) {
    // TODO: Need to somehow handle unicode in labels in this function
    if (str.size() > maxSize) {
        size_t cutPoint = maxSize / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + cutPoint + 3 + (str.size() - maxSize), str.end());
    }
    else { return std::string(maxSize - strlen_utf8(str), ' ').append(str); }
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
        return std::string((maxSize - strlen_utf8(str)) / 2, ' ')
            .append(str)
            .append(std::string(((maxSize - strlen_utf8(str)) / 2) + ((maxSize - strlen_utf8(str)) % 2), ' '));
    }
}
constexpr size_t get_axisFillerSize(size_t axisLength, size_t axisStepCount) {
    return (axisLength - axisStepCount) / (axisStepCount + 1);
}

// TODO: Also make a version where the tick positions are explictily specified in one vector of size_t
constexpr inline std::vector<std::string> create_tickMarkedAxis(std::string filler, std::string tick, size_t steps,
                                                                size_t totalWidth) {
    size_t fillerSize = get_axisFillerSize(totalWidth, steps);

    std::vector<std::string> res;
    for (size_t i_step = 0; i_step < steps; ++i_step) {
        for (size_t i_filler = 0; i_filler < fillerSize; ++i_filler) {
            res.push_back(TermColors::get_coloured(filler, Config::color_Axes_enum));
        }
        res.push_back(TermColors::get_coloured(tick, Config::color_Axes_enum));
    }
    size_t sizeOfRest = totalWidth - (steps) - (steps * fillerSize);
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
    height += (-2) - (verticalStepSize - 1);
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
        int target = value >= 1 ? (std::log10(value) / 3) : (std::log10(value) / 3) - 1;
        return {value / std::pow(1000, target), Config::si_prefixes.at(target + 10)};
    }
}

template <typename T>
requires std::is_arithmetic_v<std::decay_t<T>>
constexpr inline std::string format_toMax5length(T &&val) {
    auto [rbsed, unit] = rebase_2_SIPrefix(std::forward<decltype(val)>(val));
    return std::format("{:.{}f}{}", rbsed, rbsed >= 10 ? 0 : 1, unit.value_or(""));
}

template <typename... Ts>
requires(std::is_base_of_v<plot_structures::Base, Ts>, ...) && detail::none_sameLastLevelTypeName_v<Ts...>
constexpr inline auto generate_variantTypeMap() {
    std::unordered_map<std::string, std::variant<Ts...>> res;
    (res.insert({detail::TypeToString<Ts>(), std::variant<Ts...>(Ts())}), ...);
    return res;
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

public:
    static constexpr std::vector<std::string> drawPoints(size_t canvas_width, size_t canvas_height,
                                                         std::vector<double> const &&y_values,
                                                         std::vector<double> const &&x_values) {
        BrailleDrawer bd(canvas_width, canvas_height);

        auto [yMin, yMax] = std::ranges::minmax(y_values);
        auto [xMin, xMax] = std::ranges::minmax(x_values);

        auto yStepSize = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);
        auto xStepSize = (xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1);

        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal) {
            auto y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            auto yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            auto x       = static_cast<size_t>(((xVal - xMin) / xStepSize)) / 2;
            auto xChrPos = static_cast<size_t>(((xVal - xMin) / xStepSize)) % 2;

            bd.m_canvasBraille[y][x] |= Config::braille_map[yChrPos][xChrPos];
            bd.m_canvasColors[y][x]   = bd.m_colorPallete.front();
        };

        for (size_t i = 0; i < x_values.size(); ++i) { placePointOnCanvas(y_values[i], x_values[i]); }

        std::vector<std::string> res;
        for (int rowID = bd.m_canvasBraille.size() - 1; rowID > -1; --rowID) {
            std::u32string oneLine;
            for (size_t colID = 0; colID < bd.m_canvasBraille.front().size(); ++colID) {
                if (bd.m_canvasColors[rowID][colID].empty()) { oneLine.push_back(bd.m_canvasBraille[rowID][colID]); }
                else {
                    oneLine.append(bd.m_canvasColors[rowID][colID]);
                    oneLine.push_back(bd.m_canvasBraille[rowID][colID]);
                    oneLine.append(bd.s_terminalDefault);
                }
            }
            res.push_back(detail::convert_u32u8(oneLine));
        }

        return res;
    }
};

} // namespace detail


// Data storage for the actual data that are to be plotted
struct DataStore {
    using NLMjson = nlohmann::json;
    // The json this was constucted with/from ... possibly not strictly necessary to keep, but whatever
    std::vector<NLMjson> constructedWith;

    // TODO: Maybe provide my own 'ColType' enum ... consider
    // Data descriptors
    std::vector<std::string>                         colNames;
    std::vector<std::pair<NLMjson::value_t, size_t>> colTypes; // First =  ColType, Second = ID in data vector

    // Actual data storage
    std::vector<std::vector<std::string>> stringCols;
    std::vector<std::vector<long long>>   llCols; // Don't care about signed unsigned, etc. ... all will be long long
    std::vector<std::vector<double>>      doubleCols;


    // CONSTRUCTION
    DataStore(std::vector<NLMjson> &&jsonVec) : constructedWith(std::move(jsonVec)) {

        // Create the requisite data descriptors and the structure
        for (auto const &[key, val] : constructedWith.front().items()) {
            colNames.push_back(key);

            if (val.type() == NLMjson::value_t::string) {
                colTypes.push_back({NLMjson::value_t::string, stringCols.size()});
                stringCols.push_back(std::vector<std::string>());
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                colTypes.push_back({NLMjson::value_t::number_float, doubleCols.size()});
                doubleCols.push_back(std::vector<double>());
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                colTypes.push_back({val.type(), llCols.size()});
                llCols.push_back(std::vector<long long>());
            }
        }
        append_data(constructedWith);
    }

    // APPENDING
    void append_data(std::vector<NLMjson> const &toAppend) {
        for (auto const &oneJson : toAppend) {
            auto oneJsonIT = oneJson.items().begin();
            for (auto const &colTypesPair : colTypes) {
                if (colTypesPair.first == NLMjson::value_t::string) {
                    stringCols[colTypesPair.second].push_back(oneJsonIT.value());
                }
                else if (colTypesPair.first == NLMjson::value_t::number_integer ||
                         colTypesPair.first == NLMjson::value_t::number_unsigned) {
                    llCols[colTypesPair.second].push_back(static_cast<long long>(oneJsonIT.value()));
                }
                else if (colTypesPair.first == NLMjson::value_t::number_float) {
                    doubleCols[colTypesPair.second].push_back(oneJsonIT.value());
                }
                ++oneJsonIT;
            }
        }
    }
    void append_jsonAndData(std::vector<NLMjson> const &toAppend) {
        constructedWith.insert(constructedWith.end(), toAppend.begin(), toAppend.end());
        append_data(toAppend);
    }
};

// Encapsulates parsing of the input into DataStore
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
struct Parser {
    using NLMjson = nlohmann::json;
    static bool validate_jsonSameness(std::vector<NLMjson> const &jsonVec) {
        // Validate that all the JSON objects parsed above have the same structure
        for (auto const &js : jsonVec) {
            // Different number of items in this line vs the firstline
            if (js.size() != jsonVec.front().size()) { return false; } // throw something here

            auto firstLineIT = jsonVec.front().items().begin();
            for (auto const &[key, val] : js.items()) {

                // Key is not the same as in the first line
                if (key != firstLineIT.key()) { return false; } // throw something here

                // Type is not the same as in the first line
                if (val.type() != firstLineIT.value().type()) { return false; } // throw something here
                ++firstLineIT;
            }
        }
        return true;
    }

    static bool validate_jsonSameness(NLMjson const &json_A, NLMjson const &json_B) {
        if (json_A.size() != json_B.size()) { return false; }

        auto json_A_IT = json_A.items().begin();
        auto json_B_IT = json_B.items().begin();
        for (size_t i = 0; i < json_A.size(); ++i) {
            if (json_A_IT.key() != json_B_IT.key()) { return false; }
            if (json_B_IT.value().type() != json_B_IT.value().type()) { return false; }
            json_A_IT++, json_B_IT++;
        }
        return true;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::vector<NLMjson> parse_NDJSON(T const &stringLike) {

        // Trim input 'string like' of all 'newline' chars at the end
        auto const it =
            std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(), [](auto &&chr) { return chr == '\n'; });
        std::string_view const trimmed(stringLike.begin(), stringLike.end() - (it - stringLike.rbegin()));

        std::vector<NLMjson> parsed;
        for (auto const &oneLine : std::views::split(trimmed, '\n') |
                                       std::views::transform([](auto const &in) { return std::string_view(in); })) {
            NLMjson oneLineJson;
            try {
                oneLineJson = NLMjson::parse(oneLine);
            }
            catch (const NLMjson::exception &e) {
                // TODO: Finally figure out how to handle exceptions somewhat professionally
                std::print("{}\n", e.what());
            }
            parsed.push_back(std::move(oneLineJson));
        }
        return parsed;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static void parse_NDJSON_andAddTo(T const &stringLike, DataStore &out_DS_toAppend) {

        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here
        if (not validate_jsonSameness(parsed.front(), out_DS_toAppend.constructedWith.front())) {
        } // Throw something here

        out_DS_toAppend.append_jsonAndData(parsed);
    }


    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static DataStore parse_NDJSON_intoDS(T const &stringLike) {

        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here

        return DataStore(std::move(parsed));
    }

    template <typename T>
    requires std::is_convertible_v<typename T::value_type, std::string_view>
    static DataStore parse_NDJSON(T containerOfStringLike) {

        for (auto &oneStr : containerOfStringLike) {
            while (oneStr.back() == '\n') { oneStr.popback(); }
        }

        std::vector<NLMjson> parsed;
        for (auto const &contItem : containerOfStringLike) {
            for (auto const oneLine : std::views::split(contItem, '\n') |
                                          std::views::transform([](auto const &in) { return std::string_view(in); })) {

                NLMjson oneLineJson;
                try {
                    oneLineJson = NLMjson::parse(oneLine);
                }
                catch (const NLMjson::exception &e) {
                    std::print("{}\n", e.what());
                }
                parsed.push_back(std::move(oneLineJson));
            }
        }
        return DataStore(std::move(parsed));
    }
};

// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'
class DesiredPlot {
    using NLMjson = nlohmann::json;

private:
    static std::expected<DesiredPlot, Unexp_plotSpecs> transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                  DataStore const &ds) {
        if (dp.label_colName.has_value()) {
            auto it = std::ranges::find(ds.colNames, dp.label_colName.value());
            if (it == ds.colNames.end()) { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }
            else if (not dp.label_colID.has_value()) { dp.label_colID = it - ds.colNames.begin(); }
            else if ((it - ds.colNames.begin()) == dp.label_colID.value()) { dp.label_colName = std::nullopt; }
            else { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }
        }

        for (auto const &v_colName : dp.values_colNames) {
            auto it = std::ranges::find(ds.colNames, v_colName);
            if (it == ds.colNames.end()) { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }

            auto it2 = std::ranges::find(dp.values_colIDs, it - ds.colNames.begin());
            if (it2 == dp.values_colIDs.end()) { dp.values_colIDs.push_back(it2 - dp.values_colIDs.begin()); }

            dp.values_colNames.clear();
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_plotType(DesiredPlot &&dp, DataStore const &ds) {
        if (dp.plot_type_name.has_value()) { return dp; }

        // Helpers
        auto   valColTypeRng = std::views::filter(ds.colTypes, [](auto &&a) {
            return (a.first == NLMjson::value_t::number_float || a.first == NLMjson::value_t::number_integer ||
                    a.first == NLMjson::value_t::number_unsigned);
        });
        size_t valCols_sz    = std::ranges::count_if(valColTypeRng, [](auto &&a) { return true; });

        auto labelColTypeRng =
            std::views::filter(ds.colTypes, [](auto &&a) { return (a.first == NLMjson::value_t::string); });
        size_t labelCols_sz = std::ranges::count_if(labelColTypeRng, [](auto &&a) { return true; });

        // Actual decision making
        if (valCols_sz > Config::max_numOfValCols) { return std::unexpected(Unexp_plotSpecs::valCols); }
        // BarV
        else if (valCols_sz == 1 && labelCols_sz > 0) {
            dp.plot_type_name = detail::TypeToString<plot_structures::BarV>();
        }
        // Bubble ? Maybe later
        // else if (valCols_sz == 3) { dp.plot_type_name = detail::TypeToString<plot_structures::Bubble>(); }

        // Scatter
        else if (valCols_sz == 2 && labelCols_sz == 0) {
            dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>();
        }
        // Single line
        else if (valCols_sz == 1) { dp.plot_type_name = detail::TypeToString<plot_structures::Line>(); }
        else { dp.plot_type_name = detail::TypeToString<plot_structures::Multiline>(); }

        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_labelCol(DesiredPlot &&dp, DataStore const &ds) {
        if (dp.label_colID.has_value()) { return dp; }
        else if (dp.plot_type_name != detail::TypeToString<plot_structures::BarV>()) { return dp; }
        else {
            auto it = std::ranges::find_if(ds.colTypes, [](auto &&a) { return a.first == NLMjson::value_t::string; });
            if (it == ds.colTypes.end()) { return std::unexpected(Unexp_plotSpecs::labelCol); }
            else { dp.label_colID = it->second; }
            return dp;
        }
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_valueCols(DesiredPlot &&dp, DataStore const &ds) {
        auto addValColsUntil = [&](size_t count) -> std::expected<size_t, Unexp_plotSpecs> {
            auto getAnotherValColID = [&]() -> std::expected<size_t, Unexp_plotSpecs> {
                for (size_t i = 0; i < ds.colTypes.size(); ++i) {
                    if (ds.colTypes[i].first == NLMjson::value_t::number_float ||
                        ds.colTypes[i].first == NLMjson::value_t::number_integer ||
                        ds.colTypes[i].first == NLMjson::value_t::number_unsigned) {
                        if (std::ranges::find(dp.values_colIDs, i) == dp.values_colIDs.end()) { return i; }
                    }
                }
                // Cannot find another one
                return std::unexpected(Unexp_plotSpecs::guessValCols);
            };
            while (dp.values_colIDs.size() < count) {
                auto expID = getAnotherValColID();
                if (expID.has_value()) { dp.values_colIDs.push_back(expID.value()); }
                else { return std::unexpected(expID.error()); }
            }
            return 0uz;
        };

        // BAR PLOTS
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        // LINE PLOTS
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Line>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
            if (dp.values_colIDs.size() > 2) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        // BUBBLE PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Bubble>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(3).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_sizes(DesiredPlot &&dp, DataStore const &ds) {
        // Width always need to be provided, otherwise the whole thing doesn't work
        if (not dp.targetWidth.has_value() || dp.targetWidth.value() < 16) {
            return std::unexpected(Unexp_plotSpecs::tarWidth);
        }

        // Height can be inferred
        if (not dp.targetHeight.has_value()) { dp.targetHeight = dp.targetWidth.value() / 2; }

        // Impossible to print with height <3 under all circumstances
        if (dp.targetHeight.value() < 3) { return std::unexpected(Unexp_plotSpecs::tarWidth); }


        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_TFfeatures(DesiredPlot &&dp, DataStore const &ds) {
        if (not dp.valAxesNames_bool.has_value()) { dp.valAxesNames_bool = false; }
        if (not dp.valAxesLabels_bool.has_value()) { dp.valAxesLabels_bool = false; }
        if (not dp.valAutoFormat_bool.has_value()) { dp.valAutoFormat_bool = true; }
        if (not dp.legend_bool.has_value()) { dp.legend_bool = false; }

        return dp;
    }

public:
    std::optional<std::string> plot_type_name;

    std::optional<size_t>      label_colID; // ID in colTypes
    std::optional<std::string> label_colName;

    // TODO: Make both 'values_' into std::optional as well to keep the logic the same for all here
    std::vector<size_t>      values_colIDs; // IDs in colTypes
    std::vector<std::string> values_colNames;

    std::optional<size_t> targetHeight;
    std::optional<size_t> targetWidth;

    std::optional<bool> valAxesNames_bool;
    std::optional<bool> valAxesLabels_bool;
    std::optional<bool> valAutoFormat_bool;
    std::optional<bool> legend_bool;


    // TODO: Provide some compile time programmatic way to set the default sizes here
    DesiredPlot(std::optional<size_t> tar_width = std::nullopt, std::optional<size_t> tar_height = std::nullopt,
                std::optional<std::string> plot_type_name = std::nullopt, std::optional<size_t> l_colID = std::nullopt,
                std::optional<std::string> l_colName = std::nullopt, std::vector<size_t> v_colIDs = {},
                std::vector<std::string> v_colNames = {})
        : targetWidth(tar_width), targetHeight(tar_height), plot_type_name(std::move(plot_type_name)),
          label_colID(std::move(l_colID)), label_colName(std::move(l_colName)), values_colIDs(std::move(v_colIDs)),
          values_colNames(std::move(v_colNames)) {}

    // Create a new copy and guess_missingParams on it.
    std::expected<DesiredPlot, Unexp_plotSpecs> build_guessedParamsCPY(this DesiredPlot &self, DataStore const &ds) {
        return DesiredPlot(self).guess_missingParams(ds);
    }

    // Guesses the missing 'desired parameters' and returns a DesiredPlot with those filled in
    // Variation on a 'builder pattern'
    // Normally called 'in place' on 'DesiredPlot' instance constructed as rvalue
    // If impossible to guess or otherwise the user desires something impossible returns Err_plotSpecs.
    std::expected<DesiredPlot, Unexp_plotSpecs> guess_missingParams(this DesiredPlot &&self, DataStore const &ds) {

        // TODO: Could use std::bind for these ... had some trouble with that ... maybe return to it later.
        // Still can't quite figure it out ...  std::bind_back doesn't seem to cooperate with and_then ...

        auto gpt = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_plotType(std::forward<decltype(dp)>(dp), ds);
        };
        auto glc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_labelCol(std::forward<decltype(dp)>(dp), ds);
        };
        auto gvc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_valueCols(std::forward<decltype(dp)>(dp), ds);
        };
        auto gsz = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_sizes(std::forward<decltype(dp)>(dp), ds);
        };
        auto gtff = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_TFfeatures(std::forward<decltype(dp)>(dp), ds);
        };

        return DesiredPlot::transform_namedColsIntoIDs(std::forward<decltype(self)>(self), ds)
            .and_then(gpt)
            .and_then(glc)
            .and_then(gvc)
            .and_then(gsz)
            .and_then(gtff);
    }
};


namespace plot_structures {
// Classes derived from base represent 'plot structures' of particular types of plots (such as bar vertical, scatter
// etc.)
// Create your own 'plot structure' ie. type of plot by deriving from 'Base' class (or from other classes derived
// from it) and overriding pure virtual functions. The types properly derived from 'Base' can then be used inside
// 'PlotDrawer' inside std::variant<...>. The idea is to be able to easily customize and also possibly 'partially
// customize' as needed You always have to make the 'Base' class a friend ... this enables really nice static
// polymorphism coupled with 'deducing this' feature of C++23
class Base {
protected:
    // Descriptors - First thing to be computed.
    // BEWARE: The sizes here are 'as displayed' not the 'size in bytes' ... need to account for UTF8
    size_t areaWidth = 0, areaHeight = 0;
    size_t labels_verLeftWidth = 0, labels_verRightWidth = 0;


    size_t axis_verLeftSteps = 0, axis_varRightSteps = 0, axis_horTopSteps = 0, axis_horBottomSteps = 0;

    size_t pad_left = 2, pad_right = 0, pad_top = 0, pad_bottom = 0;

    bool labels_horTop_bool = false, labels_horBottom_bool = false;
    bool axisName_horTop_bool = false, axisName_horBottom_bool = false;

    bool axisName_verLeft_bool = false, axisName_verRight_bool = false;


    // Actual structure
    std::string              axisName_verLeft;
    std::vector<std::string> labels_verLeft;
    std::vector<std::string> axis_verLeft;

    std::string              axisName_verRight;
    std::vector<std::string> axis_verRight;
    std::vector<std::string> labels_verRight;

    std::string              axisName_horTop;
    std::string              label_horTop;
    std::vector<std::string> axis_horTop;

    std::vector<std::string> axis_horBottom;
    std::string              label_horBottom;
    std::string              axisName_horBottom;

    std::vector<std::string> corner_topLeft;
    std::vector<std::string> corner_bottomLeft;
    std::vector<std::string> corner_bottomRight;
    std::vector<std::string> corner_topRight;

    std::vector<std::string> plotArea;

    // Compute size in bytes (for reserving the output str size), not size in 'displayed characters'
    size_t compute_lengthOfSelf() const {

        size_t lngth = pad_top + pad_bottom;

        // Horizontal top axis name and axis labels lines
        lngth += axisName_horTop_bool ? (axisName_horTop.size() + corner_topLeft.front().size() +
                                         corner_topRight.front().size() + pad_left + pad_right)
                                      : 0;
        lngth += labels_horTop_bool ? (label_horTop.size() + corner_topLeft.front().size() +
                                       corner_topRight.front().size() + pad_left + pad_right)
                                    : 0;

        // First and last vertical labels + padding + vert axes names if present
        lngth += labels_verLeft.front().size() + labels_verRight.front().size() + labels_verLeft.back().size() +
                 labels_verRight.back().size();

        // The 'area corner symbols'
        lngth += Config::areaCorner_tl.size() + Config::areaCorner_tr.size() + Config::areaCorner_bl.size() +
                 Config::areaCorner_br.size();

        // All top and bottom axes
        for (int i = 0; i < areaWidth; i++) { lngth += (axis_horTop.at(i).size() + axis_horBottom.at(i).size()); }

        // Main plot area
        for (int i = 0; i < areaHeight; ++i) {
            lngth += labels_verLeft.at(i + 1).size() + labels_verRight.at(i + 1).size();
            lngth += axis_verLeft.at(i).size();
            lngth += axis_verRight.at(i).size();
            lngth += plotArea.at(i).size();
        }

        lngth += ((areaHeight + 2) * (pad_left + pad_right + (Config::axis_verName_width_vl * (axisName_verLeft_bool)) +
                                      (Config::axis_verName_width_vr * (axisName_verRight_bool))));

        // Horizontal bottom axis name and axis labels lines
        lngth += labels_horBottom_bool ? (label_horBottom.size() + corner_bottomLeft.front().size() +
                                          corner_bottomRight.front().size() + pad_left + pad_right)
                                       : 0;
        lngth += axisName_horBottom_bool ? (axisName_horBottom.size() + corner_bottomLeft.front().size() +
                                            corner_bottomRight.front().size() + pad_left + pad_right)
                                         : 0;
        return lngth;
    }

public:
    // This needs to get called after default construction
    auto build_self(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::remove_cvref_t<decltype(self)> {
        // Can only build it from rvalue ...
        if constexpr (std::is_lvalue_reference_v<decltype(self)>) { static_assert(false); }

        using expOfSelf_t = std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer>;

        auto c_dsc = [&](auto &&ps) -> expOfSelf_t { return ps.compute_descriptors(dp, ds); };
        auto v_dsc = [&](auto &&ps) -> expOfSelf_t { return ps.validate_descriptors(dp, ds); };

        auto c_anvl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_vl(dp, ds); };
        auto c_anvr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_vr(dp, ds); };

        auto c_lvl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_vl(dp, ds); };
        auto c_lvr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_vr(dp, ds); };

        auto c_avl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_vl(dp, ds); };
        auto c_avr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_vr(dp, ds); };

        auto c_ctl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_tl(dp, ds); };
        auto c_cbl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_bl(dp, ds); };
        auto c_cbr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_br(dp, ds); };
        auto c_ctr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_tr(dp, ds); };

        auto c_aht  = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_ht(dp, ds); };
        auto c_anht = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_ht(dp, ds); };
        auto c_alht = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_ht(dp, ds); };

        auto c_ahb  = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_hb(dp, ds); };
        auto c_anhb = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_hb(dp, ds); };
        auto c_alhb = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_hb(dp, ds); };

        auto c_ap = [&](auto &&ps) -> expOfSelf_t { return ps.compute_plot_area(dp, ds); };

        auto res = c_dsc(std::move(self))
                       .and_then(v_dsc)
                       .and_then(c_anvl)
                       .and_then(c_anvr)
                       .and_then(c_lvl)
                       .and_then(c_lvr)
                       .and_then(c_avl)
                       .and_then(c_avr)
                       .and_then(c_ctl)
                       .and_then(c_cbl)
                       .and_then(c_cbr)
                       .and_then(c_ctr)
                       .and_then(c_aht)
                       .and_then(c_anht)
                       .and_then(c_alht)
                       .and_then(c_ahb)
                       .and_then(c_anhb)
                       .and_then(c_alhb)
                       .and_then(c_ap);

        return res.value();
    }

    // TODO: Implement 'valiate_self()' ... consider if it is even needed or if its not already done elsewhere
    bool validate_self() const { return true; }


    std::string build_plotAsString() const {
        std::string result;
        result.reserve(compute_lengthOfSelf());

        // Add padding on top
        for (int i = 0; i < pad_top; ++i) { result.push_back('\n'); }

        // Build the heading lines of the plot
        if (axisName_horTop_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_topLeft.front());
            result.append(label_horTop);
            result.append(corner_topRight.front());
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }
        if (labels_horTop_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_topLeft.back());
            result.append(label_horTop);
            result.append(corner_topRight.back());
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        // Build horizontal top axis line
        result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), ' '));
        result.append(labels_verLeft.front());
        result.append(Config::color_Axes);
        result.append("┌");
        for (auto const &toAppend : axis_horTop) { result.append(toAppend); }
        result.append(Config::color_Axes);
        result.append("┐");
        result.append(Config::term_setDefault);
        result.append(labels_verRight.front());
        result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), ' '));
        result.push_back('\n');

        // Add plot area lines
        for (size_t i = 0; i < areaHeight; ++i) {
            result.append(std::string(pad_left, ' '));
            if (axisName_verLeft_bool) { result.push_back(axisName_verLeft.at(i)); }
            result.append(std::string((Config::axis_verName_width_vl - 1) * axisName_verLeft_bool, ' '));
            result.append(labels_verLeft.at(i + 1));
            result.append(axis_verLeft.at(i));
            result.append(plotArea.at(i));
            result.append(axis_verRight.at(i));
            result.append(labels_verRight.at(i + 1));
            result.append(std::string((Config::axis_verName_width_vr - 1) * axisName_verRight_bool, ' '));
            if (axisName_verRight_bool) { result.push_back(axisName_verRight.at(i)); }
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        // Add horizontal bottom axis line
        result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), ' '));
        result.append(labels_verLeft.back());
        result.append(Config::color_Axes);
        result.append("└");
        for (auto const &toAppend : axis_horBottom) { result.append(toAppend); }
        result.append(Config::color_Axes);
        result.append("┘");
        result.append(Config::term_setDefault);
        result.append(labels_verRight.back());
        result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), ' '));
        result.push_back('\n');

        // Add the bottom lines of the plot
        if (labels_horBottom_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_bottomLeft.front());
            result.append(label_horBottom);
            result.append(corner_bottomRight.front());
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }
        if (axisName_horBottom_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_bottomLeft.back());
            result.append(axisName_horBottom);
            result.append(corner_bottomRight.back());
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        // Add padding on bottom
        for (int i = 0; i < pad_bottom; ++i) { result.push_back('\n'); }
        return result;
    }

private:
    // TODO: Implement validate_descriptors for 'plot_structures'
    auto validate_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }

    // One needs to define all of these in a derived class.
    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
};

class BarV : public Base {
    friend class Base;

    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        // Vertical left labels
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            auto const &labelColRef = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
            auto const  labelSizes =
                std::views::transform(labelColRef, [](auto const &a) { return detail::strlen_utf8(a); });

            // TODO: Convert the 'hard limit' into some sort of constexpr config thing
            self.labels_verLeftWidth =
                std::min(Config::axisLabels_maxLength_vl,
                         std::min(std::ranges::max(labelSizes) + 1,
                                  (dp.targetWidth.value() - self.pad_left - self.pad_right) / 4));
        }
        else { self.labels_verLeftWidth = Config::max_valLabelSize; }

        // TODO: Vertical right labels ... probably nothing so keeping 0 size
        // ...

        // Vertical axes names ... LEFT always, RIGHT never
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axisName_verLeft_bool  = false;
            self.axisName_verRight_bool = false;
        }
        else {
            self.axisName_verLeft_bool  = true;
            self.axisName_verRight_bool = false;
        }


        // Plot area width (-2 is for the 2 vertical axes positions)
        self.areaWidth = dp.targetWidth.value() - self.pad_left -
                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2 -
                         self.labels_verRightWidth - (Config::axis_verName_width_vl * self.axisName_verRight_bool) -
                         self.pad_right;

        // Labels and axis name bottom
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>() ||
            (dp.plot_type_name == detail::TypeToString<plot_structures::Line>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) &&
                true) {} // TODO: Proper assessment for Line and ML
        else {
            self.labels_horBottom_bool   = true;
            self.axisName_horBottom_bool = true;
        }

        // Labels and axis name top ... probably nothing so keeping 0 size
        // ...

        // Plot area height (-2 is for the 2 horizontal axes positions)
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.areaHeight = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second).size();
        }
        else {
            self.areaHeight = dp.targetHeight.value() - self.pad_top - self.axisName_horTop_bool -
                              self.labels_horTop_bool - 2 - self.labels_horBottom_bool - self.axisName_horBottom_bool -
                              self.pad_bottom;
        }

        // Axes steps
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axis_verLeftSteps = self.areaHeight;
        }
        else { self.axis_verLeftSteps = detail::guess_stepsOnVerAxis(self.areaHeight); }


        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.axis_horBottomSteps = self.areaWidth;
        }
        else { self.axis_horBottomSteps = detail::guess_stepsOnHorAxis(self.areaWidth); }

        // Top and Right axes steps keeping as-is

        return self;
    }

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_verLeft_bool) {
            if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
                self.axisName_verLeft =
                    detail::trim2Size_leadingEnding(ds.colNames.at(dp.label_colID.value()), self.areaHeight);
            }
            else {
                self.axisName_verLeft =
                    detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.front()), self.areaHeight);
            }
        }

        return self;
    }
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        auto const &labelsRef = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
        self.labels_verLeft.push_back(std::string(self.labels_verLeftWidth, ' '));
        for (auto const &rawLabel : labelsRef) {
            self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth - 1));
            self.labels_verLeft.back().push_back(' ');
        }
        self.labels_verLeft.push_back(std::string(self.labels_verLeftWidth, ' '));
        return (self);
    }
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        self.labels_verRight.push_back("");
        for (int i = 0; i < self.areaHeight; ++i) { self.labels_verRight.push_back(""); }
        self.labels_verRight.push_back("");
        return self;
    }

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axis_verLeft = detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l, self.areaHeight,
                                                              self.areaHeight);
        }
        // All else should have vl axis ticks according to numeric values
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
            auto tmpAxis = detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l,
                                                         self.axis_verLeftSteps, self.areaHeight);
            std::ranges::reverse(tmpAxis);
            self.axis_verLeft = std::move(tmpAxis);
        }

        return self;
    }
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (false) {}
        else { self.axis_verRight = std::vector(self.areaHeight, std::string(" ")); }
        return self;
    }

    // All corners are simply empty as default ... but can possibly be used for something later if overrided in
    // derived
    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horTop_bool) {
            self.corner_topLeft.push_back(std::string(
                self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool), ' '));
        }
        if (self.labels_horTop_bool) {
            self.corner_topLeft.push_back(std::string(
                self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool), ' '));
        }

        return self;
    }
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horBottom_bool) {
            self.corner_bottomLeft.push_back(std::string(
                self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool), ' '));
        }
        if (self.labels_horBottom_bool) {
            self.corner_bottomLeft.push_back(std::string(
                self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool), ' '));
        }

        return self;
    }
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horTop_bool) {
            self.corner_topRight.push_back(std::string(
                self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool), ' '));
        }
        if (self.labels_horTop_bool) {
            self.corner_topRight.push_back(std::string(
                self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool), ' '));
        }

        return self;
    }
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horBottom_bool) {
            self.corner_bottomRight.push_back(std::string(
                self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool), ' '));
        }
        if (self.labels_horBottom_bool) {
            self.corner_bottomRight.push_back(std::string(
                self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool), ' '));
        }

        return self;
    }

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        self.axis_horTop = std::vector(self.areaWidth, std::string(" "));

        return self;
    }
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }


    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.axis_horBottom =
                detail::create_tickMarkedAxis(Config::axisFiller_b, Config::axisTick_b, self.areaWidth, self.areaWidth);
        }
        else {
            // Axis with ticks is contructed according to num of 'steps' which is the num of ticks and the areaWidth
            self.axis_horBottom = detail::create_tickMarkedAxis(Config::axisFiller_b, Config::axisTick_b,
                                                                self.axis_horBottomSteps, self.areaWidth);
        }
        return self;
    }
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            // Name of the LABEL column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.label_colID.value()), self.areaWidth);
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            // Name of the FIRST value column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.front()), self.areaWidth);
        }
        else {
            // Name of the SECOND value column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.at(1)), self.areaWidth);
        }
        return self;
    }
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computeLabels = [&](auto const &valColRef) -> void {
            size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            auto   stepSize         = (maxV - minV) / (self.areaWidth + 1);
            size_t placedChars      = 0;

            // Construct the [0:0] point label
            std::string tempStr = detail::format_toMax5length(minV);
            self.label_horBottom.append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);

            // Construct the tick labels
            for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
                while (placedChars < (i * (fillerSize + 1) + fillerSize)) {
                    self.label_horBottom.push_back(' ');
                    placedChars++;
                }
                tempStr = detail::format_toMax5length(minV + ((i * (fillerSize + 1) + fillerSize) * stepSize));
                self.label_horBottom.append(tempStr);
                placedChars += detail::strlen_utf8(tempStr);
            }

            // Construct the [0:end] point label
            tempStr = detail::format_toMax5length(maxV);
            for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
                self.label_horBottom.push_back(' ');
            }
            self.label_horBottom.append(tempStr);
        };
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.label_horBottom = std::string(self.areaWidth + 2, ' ');
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Line>() ||
                 dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            // TODO: What to do with Line and Multiline axisLabel bottom
        }
        else {
            auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
            if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
                auto const &valColRef = ds.doubleCols.at(valColTypeRef.second);
                computeLabels(valColRef);
            }
            else {
                auto const &valColRef = ds.llCols.at(valColTypeRef.second);
                computeLabels(valColRef);
            }
        }
        return self;
    }

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computePA = [&]<typename T>(T const &valColRef) -> void {
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            long long scalingFactor;
            if constexpr (std::is_integral_v<std::decay_t<typename T::value_type>>) {
                scalingFactor = LONG_LONG_MAX / (std::max(std::abs(maxV), std::abs(minV)));
            }
            else if constexpr (std::is_floating_point_v<std::decay_t<typename T::value_type>>) { scalingFactor = 1; }
            else { static_assert(false); } // Can't plot non-numeric values

            auto maxV_adj = maxV * scalingFactor;
            auto minV_adj = minV * scalingFactor;
            auto stepSize = (maxV_adj - minV_adj) / (self.areaWidth + 1);

            for (auto const &val : valColRef) {
                self.plotArea.push_back(std::string());
                long long rpt = (val * scalingFactor - minV_adj) / stepSize;
                self.plotArea.back().append(Config::color_Vals1);
                for (long long i = rpt; i > 0; --i) { self.plotArea.back().append("■"); }
                self.plotArea.back().append(Config::term_setDefault);
                for (long long i = rpt; i < self.areaWidth; ++i) { self.plotArea.back().push_back(' '); }
            }
        };

        auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
            computePA(ds.doubleCols.at(valColTypeRef.second));
        }
        else { computePA(ds.llCols.at(valColTypeRef.second)); }
        return self;
    }
};

class BarH : public BarV {
    friend class Base;
};

class Line : public BarV {
    friend class Base;
};

class Multiline : public Line {
    friend class Base;
};

class Scatter : public BarV {
    friend class Base;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        // TODO: Special logic for value labels of vertical axes
        return (self);
    }

    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computeLabels = [&](auto const &valColRef) -> void {
            size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            auto   stepSize         = ((maxV - minV) / ((2 * self.areaWidth) + 1)) * 2;
            size_t placedChars      = 0;

            // Construct the [0:0] point label
            std::string tempStr = detail::format_toMax5length(minV - stepSize);
            self.label_horBottom.append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);

            // Construct the tick labels
            for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
                while (placedChars < (i * (fillerSize + 1) + fillerSize)) {
                    self.label_horBottom.push_back(' ');
                    placedChars++;
                }
                tempStr =
                    detail::format_toMax5length((minV - stepSize) + ((i * (fillerSize + 1) + fillerSize) * stepSize));
                self.label_horBottom.append(tempStr);
                placedChars += detail::strlen_utf8(tempStr);
            }

            // Construct the [0:end] point label
            tempStr = detail::format_toMax5length(maxV + (stepSize / 2));
            for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
                self.label_horBottom.push_back(' ');
            }
            self.label_horBottom.append(tempStr);
        };
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.label_horBottom = std::string(self.areaWidth + 2, ' ');
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Line>() ||
                 dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            // TODO: What to do with Line and Multiline axisLabel bottom
        }
        else {
            // The SECOND value column is plotted on hb axis
            auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.at(1));
            if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
                auto const &valColRef = ds.doubleCols.at(valColTypeRef.second);
                computeLabels(valColRef);
            }
            else {
                auto const &valColRef = ds.llCols.at(valColTypeRef.second);
                computeLabels(valColRef);
            }
        }
        return self;
    }

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computePA = [&]<typename T>(T const &valColRef) -> void {
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            long long scalingFactor;
            if constexpr (std::is_integral_v<std::decay_t<typename T::value_type>>) {
                scalingFactor = LONG_LONG_MAX / (std::max(std::abs(maxV), std::abs(minV)));
            }
            else if constexpr (std::is_floating_point_v<std::decay_t<typename T::value_type>>) { scalingFactor = 1; }
            else { static_assert(false); } // Can't plot non-numeric values

            auto maxV_adj = maxV * scalingFactor;
            auto minV_adj = minV * scalingFactor;
            auto stepSize = (maxV_adj - minV_adj) / (self.areaWidth + 1);

            for (auto const &val : valColRef) {
                self.plotArea.push_back(std::string());
                long long rpt = (val * scalingFactor - minV_adj) / stepSize;
                self.plotArea.back().append(Config::color_Vals1);
                for (long long i = rpt; i > 0; --i) { self.plotArea.back().append("■"); }
                self.plotArea.back().append(Config::term_setDefault);
                for (long long i = rpt; i < self.areaWidth; ++i) { self.plotArea.back().push_back(' '); }
            }
        };

        auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
            computePA(ds.doubleCols.at(valColTypeRef.second));
        }
        else { computePA(ds.llCols.at(valColTypeRef.second)); }
        return self;
    }
};

class Bubble : public Scatter {
    friend class Base;
};

} // namespace plot_structures


template <typename PS_VAR>
class PlotDrawer {
private:
    PS_VAR m_ps_var;

public:
    constexpr PlotDrawer() {};
    PlotDrawer(auto ps_var, DesiredPlot const &dp, DataStore const &ds) {
        auto ol = [&](auto &&var) {
            m_ps_var = std::move(var).build_self(dp, ds);
            int a    = 0;
        };
        std::visit(ol, m_ps_var);
    }

    void update_newPlotStructure(DesiredPlot const &dp, DataStore const &ds) {
        auto ol = [&](auto &&var) { m_ps_var = decltype(var)().build_self(dp, ds); };
        std::visit(ol, m_ps_var);
    }

    bool validate_self() const {
        auto validate = [&](auto &&var) -> bool { return var.validate_self(); };
        return std::visit(validate, m_ps_var);
    }

    std::expected<std::string, Unexp_plotDrawer> validateAndDrawPlot() const {
        // TODO: Add some validation before drawing
        if (validate_self() == false) { return std::unexpected(Unexp_plotDrawer::plotStructureInvalid); }
        else { return drawPlot(); }
    }

    std::string drawPlot() const {
        auto ol = [&](auto &&var) -> std::string { return var.build_plotAsString(); };
        return std::visit(ol, m_ps_var);
    }
};


// This is a map of default constructed 'plot_structures' inside an std::variant
// Pass the 'plot_structure' template types that should be used by the library
// This is the only place where one 'selects' these template types
static inline const auto mp_names2Types =
    detail::generate_variantTypeMap<plot_structures::BarV, plot_structures::BarH, plot_structures::Line,
                                    plot_structures::Multiline, plot_structures::Scatter, plot_structures::Bubble>();

inline auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds) {
    auto ref          = mp_names2Types.at(dp.plot_type_name.value());
    using varType     = decltype(ref);
    auto overload_set = [&](auto const &variantItem) -> PlotDrawer<varType> {
        return PlotDrawer<varType>(variantItem, dp, ds);
    };
    return std::visit(overload_set, ref);
}


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;