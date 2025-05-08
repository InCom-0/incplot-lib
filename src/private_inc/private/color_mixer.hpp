#pragma once

#include <array>
#include <vector>

#include <incplot/config.hpp>

namespace incom {
namespace terminal_plot {
namespace detail {

class ColorMixer {
private:
    struct C {
        unsigned int r = 0, g = 0, b = 0;
    };
    struct C_StepSize {
        double r = 1.0, g = 1.0, b = 1.0;
    };

public:
    std::vector<C>            m_selColors;
    std::vector<C_StepSize>   m_stepSize_perColor;
    std::vector<size_t> const m_maxSteps_perColor;

    C       m_maxRGBVals;
    C       m_minRGBVals;
    C       m_fakeZeroColor;
    C const m_blackColor;

    ColorMixer(std::vector<size_t> maxSteps_perColor, size_t num_colorsToSelect = 3uz,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_defaulRaw.at(0),
                                                                            Config::colors_defaulRaw.at(1),
                                                                            Config::colors_defaulRaw.at(2)},
               std::array<unsigned int, 3> blackRGB                      = Config::colors_blackRaw);


    ColorMixer(size_t num_colorsToSelect                                 = 3uz,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {
                   Config::colors_defaulRaw.at(0), Config::colors_defaulRaw.at(1), Config::colors_defaulRaw.at(2)});

    // Computes how many dots of the same category/color are placed in the same dot position
    // Returns vector representing the maximum per color across the whole plot
    static std::vector<size_t> compute_maxStepsPerColor(
        std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> const &colorPointCounts);

    std::array<unsigned int, 3> compute_colorOfPosition(
        std::array<std::array<std::vector<size_t>, 2>, 4> const &colorPointCounts_onePos);
};

} // namespace detail
} // namespace terminal_plot
} // namespace incom