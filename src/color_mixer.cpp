
#include <algorithm>
#include <private/color_mixer.hpp>
#include <ranges>


namespace incom {
namespace terminal_plot {
namespace detail {
using namespace incstd::color;

ColorMixer::ColorMixer(std::vector<size_t> maxSteps_perColor, size_t num_colorsToSelect,
                       palette16 const &selectColorsFrom, inc_sRGB blackRGB)
    : m_maxSteps_perColor(std::move(maxSteps_perColor)), m_blackColor(blackRGB),
      m_stepSize_perColor(num_colorsToSelect, C_StepSize()) {

    maxSteps_perColor.resize(num_colorsToSelect);

    // Construct vector of actual colors (in RGB)
    for (size_t fromColID = 0; fromColID < num_colorsToSelect; ++fromColID) {
        m_selColors.push_back(selectColorsFrom.at(fromColID));
    }

    // Compute max RGB per channel
    m_maxRGBVals.r = (*std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.r < r.r; })).r;
    m_maxRGBVals.g = (*std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.g < r.g; })).g;
    m_maxRGBVals.b = (*std::ranges::max_element(m_selColors, [](auto &&l, auto &&r) { return l.b < r.b; })).b;

    m_minRGBVals.r = (*std::ranges::min_element(m_selColors, [](auto &&l, auto &&r) { return l.r < r.r; })).r;
    m_minRGBVals.g = (*std::ranges::min_element(m_selColors, [](auto &&l, auto &&r) { return l.g < r.g; })).g;
    m_minRGBVals.b = (*std::ranges::min_element(m_selColors, [](auto &&l, auto &&r) { return l.b < r.b; })).b;

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

ColorMixer::ColorMixer(size_t num_colorsToSelect, palette16 selectColorsFrom, inc_sRGB blackRGB)
    : ColorMixer(std::vector<size_t>(selectColorsFrom.size(), 1), num_colorsToSelect, selectColorsFrom,
                 selectColorsFrom.front()) {}

std::vector<size_t> ColorMixer::compute_maxStepsPerColor(
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

incstd::color::inc_sRGB ColorMixer::compute_colorOfPosition(
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

    incstd::color::inc_sRGB res{m_fakeZeroColor.r, m_fakeZeroColor.g, m_fakeZeroColor.b};
    for (auto const &[steps, stepSize] : std::views::zip(stepsForPos_perColor, m_stepSize_perColor)) {
        res.r =
            std::max(static_cast<int>(m_minRGBVals.r), static_cast<int>(res.r) + static_cast<int>(steps * stepSize.r));
        res.g =
            std::max(static_cast<int>(m_minRGBVals.g), static_cast<int>(res.g) + static_cast<int>(steps * stepSize.g));
        res.b =
            std::max(static_cast<int>(m_minRGBVals.b), static_cast<int>(res.b) + static_cast<int>(steps * stepSize.b));
    }
    res.r = std::min(m_maxRGBVals.r, res.r);
    res.g = std::min(m_maxRGBVals.g, res.g);
    res.b = std::min(m_maxRGBVals.b, res.b);
    return res;
}


} // namespace detail
} // namespace terminal_plot
} // namespace incom