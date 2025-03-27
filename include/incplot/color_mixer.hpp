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
    C const m_blackColor;

    ColorMixer(std::vector<size_t>                      maxSteps_perColor,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_defaulRaw.at(0),
                                                                            Config::colors_defaulRaw.at(1),
                                                                            Config::colors_defaulRaw.at(2)},
               size_t num_colorsToSelect = 3uz, std::array<unsigned int, 3> blackRGB = Config::colors_blackRaw)
        : m_maxSteps_perColor(std::move(maxSteps_perColor)), m_blackColor{blackRGB[0], blackRGB[1], blackRGB[2]},
          m_stepSize_perColor(num_colorsToSelect, C_StepSize()) {

        // Construct vector of actual colors (in RGB)
        for (auto const &inpArr : selectColorsFrom) { m_selColors.push_back(C{inpArr[0], inpArr[1], inpArr[2]}); }

        // Compute max RGB per channel
        m_maxRGBVals.r = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.r < r.r; })->r;
        m_maxRGBVals.g = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.g < r.g; })->g;
        m_maxRGBVals.b = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.b < r.b; })->b;

        // Compute stepsize per color
        for (auto const &zippedEle : std::ranges::views::zip(m_stepSize_perColor, m_maxSteps_perColor, m_selColors)) {
            std::get<0>(zippedEle).r =
                (std::get<2>(zippedEle).r - static_cast<double>(m_blackColor.r)) / (std::get<1>(zippedEle));
            std::get<0>(zippedEle).g =
                (std::get<2>(zippedEle).g - static_cast<double>(m_blackColor.g)) / (std::get<1>(zippedEle));
            std::get<0>(zippedEle).b =
                (std::get<2>(zippedEle).b - static_cast<double>(m_blackColor.b)) / (std::get<1>(zippedEle));
        }
    }

    ColorMixer(std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_defaulRaw.at(0),
                                                                            Config::colors_defaulRaw.at(1),
                                                                            Config::colors_defaulRaw.at(2)})
        : ColorMixer(std::vector<size_t>(selectColorsFrom.size(), 1), selectColorsFrom) {}


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

        std::array<unsigned int, 3> res{m_blackColor.r, m_blackColor.g, m_blackColor.b};
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