#pragma once

#include <array>
#include <cstddef>
#include <string>


namespace incom {
namespace terminal_plot {
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
    static constexpr auto _S_basicCVTScolMap = [] {
        using namespace std::literals;
        std::array<std::string_view, 108> arr{}; // all default to ""sv

        arr[0]   = "\x1b[m"sv;
        arr[1]   = "\x1b[1m"sv;
        arr[22]  = "\x1b[22m"sv;
        arr[4]   = "\x1b[4m"sv;
        arr[24]  = "\x1b[24m"sv;
        arr[7]   = "\x1b[7m"sv;
        arr[27]  = "\x1b[27m"sv;
        arr[30]  = "\x1b[30m"sv;
        arr[31]  = "\x1b[31m"sv;
        arr[32]  = "\x1b[32m"sv;
        arr[33]  = "\x1b[33m"sv;
        arr[34]  = "\x1b[34m"sv;
        arr[35]  = "\x1b[35m"sv;
        arr[36]  = "\x1b[36m"sv;
        arr[37]  = "\x1b[37m"sv;
        arr[38]  = "\x1b[38m"sv;
        arr[39]  = "\x1b[39m"sv;
        arr[40]  = "\x1b[40m"sv;
        arr[41]  = "\x1b[41m"sv;
        arr[42]  = "\x1b[42m"sv;
        arr[43]  = "\x1b[43m"sv;
        arr[44]  = "\x1b[44m"sv;
        arr[45]  = "\x1b[45m"sv;
        arr[46]  = "\x1b[46m"sv;
        arr[47]  = "\x1b[47m"sv;
        arr[48]  = "\x1b[48m"sv;
        arr[49]  = "\x1b[49m"sv;
        arr[90]  = "\x1b[90m"sv;
        arr[91]  = "\x1b[91m"sv;
        arr[92]  = "\x1b[92m"sv;
        arr[93]  = "\x1b[93m"sv;
        arr[94]  = "\x1b[94m"sv;
        arr[95]  = "\x1b[95m"sv;
        arr[96]  = "\x1b[96m"sv;
        arr[97]  = "\x1b[97m"sv;
        arr[100] = "\x1b[100m"sv;
        arr[101] = "\x1b[101m"sv;
        arr[102] = "\x1b[102m"sv;
        arr[103] = "\x1b[103m"sv;
        arr[104] = "\x1b[104m"sv;
        arr[105] = "\x1b[105m"sv;
        arr[106] = "\x1b[106m"sv;
        arr[107] = "\x1b[107m"sv;

        return arr;
    }();

public:
    // Class should be impossible to instantiate
    TermColors()                    = delete;
    void *operator new(std::size_t) = delete;

    static constexpr auto get_basicColor(Color_CVTS const col) {
        return _S_basicCVTScolMap.at(static_cast<size_t>(col));
    }
    static constexpr std::string get_basicColor_str(Color_CVTS const col) {
        return std::string{get_basicColor(col)};
    }

    // Get color by ASCI defined fixed 256 palette
    static constexpr auto get_fgColor(int const color_256) {
        return std::string("\x1b[38;5;").append(std::to_string(color_256)).append("m");
    }
    static constexpr auto get_bgColor(int const color_256) {
        return std::string("\x1b[48;5;").append(std::to_string(color_256)).append("m");
    }

    // Get color by RGB composition
    static constexpr auto get_fgColor(unsigned int r, unsigned int g, unsigned int b) {
        std::string res("\x1b[38;2;");
        res.append(std::to_string(r));
        res.push_back(';');
        res.append(std::to_string(g));
        res.push_back(';');
        res.append(std::to_string(b));
        res.push_back('m');
        return res;
    }
    static constexpr auto get_fgColor(std::array<unsigned int, 3> color) {
        return get_fgColor(color[0], color[1], color[2]);
    }


    static constexpr auto get_bgColor(unsigned int r, unsigned int g, unsigned int b) {
        std::string res("\x1b[48;2;");
        res.append(std::to_string(r));
        res.push_back(';');
        res.append(std::to_string(g));
        res.push_back(';');
        res.append(std::to_string(b));
        res.push_back('m');
        return res;
    }
    static constexpr auto get_bgColor(std::array<unsigned int, 3> color) {
        return get_bgColor(color[0], color[1], color[2]);
    }


    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_coloured(T &&toColor, Color_CVTS const col) {
        return std::string(get_basicColor(col))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_coloured(T &&toColor, int fg_color_256) {
        return std::string(get_fgColor(fg_color_256))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }
    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static constexpr auto get_coloured(T &&toColor, int fg_color_256, int bg_color_256) {
        return std::string(get_fgColor(fg_color_256))
            .append(get_bgColor(bg_color_256))
            .append(std::forward<T>(toColor))
            .append(get_basicColor(Color_CVTS::Default));
    }
};
} // namespace terminal_plot
} // namespace incom