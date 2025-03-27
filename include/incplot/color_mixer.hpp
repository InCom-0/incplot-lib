#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

#include <incplot/config.hpp>

namespace incom {
namespace terminal_plot {
namespace detail {

class ColorMixer {

public:
    struct C {
        unsigned int r = 255, g = 255, b = 255;
    };

    struct C_StepSize {
        double r = 1.0, g = 1.0, b = 1.0;
    };

    std::vector<C>          m_selColors;
    std::vector<C_StepSize> m_stepSize_perColor;
    std::vector<size_t>     m_maxSteps_perColor;

    C m_maxRGBVals{0, 0, 0};


    ColorMixer(std::vector<size_t>                      maxSteps_perColor,
               std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_DefaulRaw.at(0),
                                                                            Config::colors_DefaulRaw.at(1),
                                                                            Config::colors_DefaulRaw.at(2)},
               size_t num_colorsToSelect                                 = 3uz)
        : m_maxSteps_perColor(std::move(maxSteps_perColor)) {

        for (auto const &inpArr : selectColorsFrom) { m_selColors.push_back(C{inpArr[0], inpArr[1], inpArr[2]}); }

        m_maxRGBVals.r = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.r < r.r; })->r;
        m_maxRGBVals.g = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.g < r.g; })->g;
        m_maxRGBVals.b = std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.b < r.b; })->b;

        for (auto const &zippedEle : std::ranges::views::zip(m_stepSize_perColor, m_maxSteps_perColor, m_selColors)) {
            std::get<0>(zippedEle).r = std::get<2>(zippedEle).r / static_cast<double>(std::get<1>(zippedEle));
            std::get<0>(zippedEle).g = std::get<2>(zippedEle).g / static_cast<double>(std::get<1>(zippedEle));
            std::get<0>(zippedEle).b = std::get<2>(zippedEle).b / static_cast<double>(std::get<1>(zippedEle));
        }
    }

    ColorMixer(std::vector<std::array<unsigned int, 3>> selectColorsFrom = {Config::colors_DefaulRaw.at(0),
                                                                            Config::colors_DefaulRaw.at(1),
                                                                            Config::colors_DefaulRaw.at(2)},
               size_t num_colorsToSelect                                 = 3uz)
        : ColorMixer(std::vector<size_t>(selectColorsFrom.size(), 1), selectColorsFrom, num_colorsToSelect) {}
};

} // namespace detail
} // namespace terminal_plot
} // namespace incom