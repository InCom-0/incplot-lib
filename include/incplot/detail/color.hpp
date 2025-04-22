#pragma once

#include <ranges>
#include <array>
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

public:
    // Class should be impossible to instantiate
    TermColors()                    = delete;
    void *operator new(std::size_t) = delete;

    static constexpr auto get_basicColor(Color_CVTS const col) {
        return std::string(std::ranges::find_if(_S_basicCVTScolMap, [&](auto &&pr) {
                               return pr.first == static_cast<int>(col);
                           })->second);
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