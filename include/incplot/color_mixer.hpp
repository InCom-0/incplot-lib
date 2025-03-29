#pragma once

#include <algorithm>
#include <array>
#include <ranges>
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
    C       m_fakeZeroColor;
    C const m_blackColor;

    ColorMixer(std::vector<size_t> maxSteps_perColor, size_t num_colorsToSelect = 3uz,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_defaulRaw.at(0),
                                                                            Config::colors_defaulRaw.at(1),
                                                                            Config::colors_defaulRaw.at(2)},
               std::array<unsigned int, 3> blackRGB                      = Config::colors_blackRaw)
        : m_maxSteps_perColor(std::move(maxSteps_perColor)), m_blackColor{blackRGB[0], blackRGB[1], blackRGB[2]},
          m_stepSize_perColor(num_colorsToSelect, C_StepSize()) {

        // Construct vector of actual colors (in RGB)
        for (size_t fromColID = 0; fromColID < num_colorsToSelect; ++fromColID) {
            m_selColors.push_back(C{selectColorsFrom.at(fromColID)[0], selectColorsFrom.at(fromColID)[1],
                                    selectColorsFrom.at(fromColID)[2]});
        }

        // Compute max RGB per channel
        m_maxRGBVals.r = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.r < r.r; })->r;
        m_maxRGBVals.g = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.g < r.g; })->g;
        m_maxRGBVals.b = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.b < r.b; })->b;

        m_fakeZeroColor.r = m_blackColor.r + static_cast<unsigned int>((m_maxRGBVals.r - m_blackColor.r) *
                                                                       (1 - Config::colors_scaleDistanceFromBlack));
        m_fakeZeroColor.g = m_blackColor.g + static_cast<unsigned int>((m_maxRGBVals.g - m_blackColor.g) *
                                                                       (1 - Config::colors_scaleDistanceFromBlack));
        m_fakeZeroColor.b = m_blackColor.b + static_cast<unsigned int>((m_maxRGBVals.b - m_blackColor.b) *
                                                                       (1 - Config::colors_scaleDistanceFromBlack));

        // Compute stepsize per color
        for (auto const &zippedEle : std::ranges::views::zip(m_stepSize_perColor, m_maxSteps_perColor, m_selColors)) {
            std::get<0>(zippedEle).r =
                (std::get<2>(zippedEle).r - static_cast<double>(m_fakeZeroColor.r)) / (std::get<1>(zippedEle));
            std::get<0>(zippedEle).g =
                (std::get<2>(zippedEle).g - static_cast<double>(m_fakeZeroColor.g)) / (std::get<1>(zippedEle));
            std::get<0>(zippedEle).b =
                (std::get<2>(zippedEle).b - static_cast<double>(m_fakeZeroColor.b)) / (std::get<1>(zippedEle));
        }
    }

    ColorMixer(size_t num_colorsToSelect                                 = 3uz,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_defaulRaw.at(0),
                                                                            Config::colors_defaulRaw.at(1),
                                                                            Config::colors_defaulRaw.at(2)})
        : ColorMixer(std::vector<size_t>(selectColorsFrom.size(), 1), num_colorsToSelect, selectColorsFrom) {}

    // Compute how many dots of the same category/color are placed in the same dot position
    // Returns vector representing the maxim per color across the whole plot
    static constexpr std::vector<size_t> compute_maxStepsPerColor(
        std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> const &colorPointCounts) {
        std::vector res = std::vector<size_t>(colorPointCounts.at(0).at(0).at(0).at(0).size(), 0uz);

        for (auto const &line : colorPointCounts) {
            for (auto const &brailleGroup : line) {
                for (auto const &braille_line : brailleGroup) {
                    for (auto const &braille_pos : braille_line) {
                        for (size_t i = 0; auto const &oneColorPointCount : braille_pos) {
                            res.at(i) = std::max(res.at(i), oneColorPointCount);
                            i++;
                        }
                    }
                }
            }
        }
        return res;
    }

    //
    constexpr std::array<unsigned int, 3> compute_colorOfPosition(
        std::array<std::array<std::vector<size_t>, 2>, 4> const &colorPointCounts_onePos) {

        std::vector<size_t> stepsForPos_perColor = std::vector<size_t>(colorPointCounts_onePos.at(0).at(0).size(), 0uz);
        for (auto const &braille_line : colorPointCounts_onePos) {
            for (auto const &braille_pos : braille_line) {
                // i represents 'category id' which is also 'color id' as they are mapped 1:1
                for (size_t i = 0; auto const &oneColorPointCount : braille_pos) {
                    stepsForPos_perColor.at(i) = std::max(stepsForPos_perColor.at(i), oneColorPointCount);
                    i++;
                }
            }
        }

        std::array<unsigned int, 3> res{m_fakeZeroColor.r, m_fakeZeroColor.g, m_fakeZeroColor.b};
        for (auto const &[steps, stepSize] : std::views::zip(stepsForPos_perColor, m_stepSize_perColor)) {
            res[0] += static_cast<unsigned int>(steps * stepSize.r);
            res[1] += static_cast<unsigned int>(steps * stepSize.g);
            res[2] += static_cast<unsigned int>(steps * stepSize.b);
        }
        res[0] = std::min(m_maxRGBVals.r, res[0]);
        res[1] = std::min(m_maxRGBVals.g, res[1]);
        res[2] = std::min(m_maxRGBVals.b, res[2]);
        return res;
    }
};

} // namespace detail
} // namespace terminal_plot
} // namespace incom