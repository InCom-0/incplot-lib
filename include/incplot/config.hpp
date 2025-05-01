#pragma once

#include <array>
#include <incplot/detail/color.hpp>
#include <string>


namespace incom {
namespace terminal_plot {

class Config {
public:
    // UNICODE SYMBOLS IN PLOT
    static constexpr std::string axisTick_l = "┤";
    static constexpr std::string axisTick_b = "┬";
    static constexpr std::string axisTick_r = "├";
    static constexpr std::string axisTick_t = "┴";

    static constexpr std::string axisFiller_l = "│";
    static constexpr std::string axisFiller_b = "─";
    static constexpr std::string axisFiller_r = "│";
    static constexpr std::string axisFiller_t = "─";

    static constexpr std::string areaCorner_tl = "┬";
    static constexpr std::string areaCorner_bl = "┼";
    static constexpr std::string areaCorner_br = "┤";
    static constexpr std::string areaCorner_tr = "┐";

    static constexpr std::string areaCorner_tl_barV = "┌";
    static constexpr std::string areaCorner_bl_barV = "└";

    // 4 rows by 2 cols of braille 'single dots' for composition by 'bitwise or' into all braille chars
    static constexpr std::array<std::array<char32_t, 2>, 4> braille_map{U'⡀', U'⢀', U'⠄', U'⠠', U'⠂', U'⠐', U'⠁', U'⠈'};
    static constexpr char32_t                               braille_blank = U'⠀';
    static constexpr char                                   space         = ' ';

    static constexpr std::array<char32_t, 9> blocks_ver{U' ', U'▁', U'▂', U'▃', U'▄', U'▅', U'▆', U'▇', U'█'};
    static constexpr std::array<char32_t, 9> blocks_hor{U' ', U'▏', U'▎', U'▍', U'▌', U'▋', U'▊', U'▉', U'█'};
    static constexpr std::array<char32_t, 4> blocks_shades_LMD{U' ', U'░', U'▒', U'▓'};


    // COLORS
    static constexpr Color_CVTS color_Axes_enum  = Color_CVTS::Bright_Foreground_Black;
    static constexpr Color_CVTS color_Vals1_enum = Color_CVTS::Foreground_Green;
    static constexpr Color_CVTS color_Vals2_enum = Color_CVTS::Foreground_Blue;
    static constexpr Color_CVTS color_Vals3_enum = Color_CVTS::Foreground_Red;
    static constexpr Color_CVTS color_Vals4_enum = Color_CVTS::Foreground_Cyan;
    static constexpr Color_CVTS color_Vals5_enum = Color_CVTS::Foreground_Yellow;
    static constexpr Color_CVTS color_Vals6_enum = Color_CVTS::Foreground_Magenta;

    static constexpr std::string color_Axes  = TermColors::get_basicColor(color_Axes_enum);
    static constexpr std::string color_Vals1 = TermColors::get_basicColor(color_Vals1_enum);
    static constexpr std::string color_Vals2 = TermColors::get_basicColor(color_Vals2_enum);
    static constexpr std::string color_Vals3 = TermColors::get_basicColor(color_Vals3_enum);
    static constexpr std::string color_Vals4 = TermColors::get_basicColor(color_Vals4_enum);
    static constexpr std::string color_Vals5 = TermColors::get_basicColor(color_Vals5_enum);
    static constexpr std::string color_Vals6 = TermColors::get_basicColor(color_Vals6_enum);

    static constexpr std::array<std::array<unsigned int, 3>, 3> colors_defaulRaw{19u,  161u, 14u, 0u, 55u,
                                                                                 218u, 197u, 15u, 31u};

    static constexpr std::array<unsigned int, 3> colors_blackRaw{12, 12, 12};
    static constexpr double                      colors_scaleDistanceFromBlack = 0.55;

    // MULTILINE
    static constexpr size_t y_interpolationMultiplier = 4;
    static constexpr size_t x_interpolationMultiplier = 4;

    // OTHER PLOT SETTINGS

    static constexpr std::string noLabel = "[no label]";

    static constexpr size_t max_sizeOfValueLabels = 5uz;

    static constexpr size_t min_areaWidth  = 8uz;
    static constexpr size_t min_areaHeight = 1uz;

    static constexpr size_t max_numOfValCols       = 4uz;
    static constexpr size_t max_maxNumOfCategories = 3uz;

    static constexpr size_t axisLabels_maxLength_vl = 16uz;
    static constexpr size_t axisLabels_maxLength_vr = 16uz;
    static constexpr size_t axisLabels_padRight_vl  = 1uz;
    static constexpr size_t axisLabels_padLeft_vr   = 1uz;

    static constexpr size_t ps_padLeft   = 2;
    static constexpr size_t ps_padRight  = 0;
    static constexpr size_t ps_padTop    = 1;
    static constexpr size_t ps_padBottom = 0;

    static constexpr size_t axis_stepSize_vl = 5uz;
    static constexpr size_t axis_stepSize_vr = 5uz;

    static constexpr size_t axis_verName_width_vl = 3uz;
    static constexpr size_t axis_verName_width_vr = 3uz;

    static constexpr std::string term_setDefault = TermColors::get_basicColor(Color_CVTS::Default);

    static constexpr std::array<std::string, 21> const si_prefixes{"q", "r", "y", "z", "a", "f", "p", "n", "μ", "m", "",
                                                                   "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q"};

    // COLUMN PARAMETERS ANALYSIS SETTINGS
    static constexpr double timeSeriesIDX_allowanceUP   = 0.1;
    static constexpr double timeSeriesIDX_allowanceDOWN = 0.1;

    // TERMINAL PARAMETERS INFERENCE SETTINGS
    static constexpr int delta_toInferredWidth  = -2;
    static constexpr int delta_toInferredHeight = -4;


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
} // namespace terminal_plot
} // namespace incom