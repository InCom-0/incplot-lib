#pragma once

#include <cassert>
#include <private/detail.hpp>
#include <ranges>


namespace incom {
namespace terminal_plot {
namespace detail {
class BrailleDrawer {
private:
    std::vector<std::vector<std::u32string>>                                    m_canvasColors;
    std::vector<std::vector<char32_t>>                                          m_canvasBraille;
    std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> m_pointsCountPerPos_perColor;

    std::array<std::u32string, 6> m_colorPallete;
    std::u32string                s_terminalDefault = detail::convert_u32u8(Config::term_setDefault);

    BrailleDrawer() {};
    BrailleDrawer(size_t canvas_width, size_t canvas_height, size_t numOf_categories,
                  std::array<Color_CVTS, 6> const &colorPalette)
        : m_canvasColors(std::vector(canvas_height, std::vector<std::u32string>(canvas_width, U""))),
          m_canvasBraille(std::vector(canvas_height, std::vector<char32_t>(canvas_width, Config::braille_blank))),
          m_colorPallete{detail::convert_u32u8(TermColors::get_basicColor(colorPalette[0])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[1])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[2])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[3])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[4])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[5]))},
          m_pointsCountPerPos_perColor(std::vector(
              canvas_height,
              std::vector(canvas_width,
                          std::array<std::array<std::vector<size_t>, 2>, 4>{
                              std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz),
                              std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz),
                              std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz),
                              std::vector<size_t>(numOf_categories, 0uz),
                              std::vector<size_t>(numOf_categories, 0uz)}))) {};

    void compute_canvasColors() {
        ColorMixer cm(ColorMixer::compute_maxStepsPerColor(m_pointsCountPerPos_perColor));
        for (size_t rowID = 0; rowID < m_pointsCountPerPos_perColor.size(); ++rowID) {
            for (size_t colID = 0; colID < m_pointsCountPerPos_perColor[rowID].size(); ++colID) {
                if (m_canvasBraille[rowID][colID] != Config::braille_blank) {
                    m_canvasColors[rowID][colID] = detail::convert_u32u8(TermColors::get_fgColor(
                        cm.compute_colorOfPosition(m_pointsCountPerPos_perColor[rowID][colID])));
                }
            }
        }
    }

    std::vector<std::string> construct_outputPlotArea() {
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
    static std::pair<std::vector<double>, std::vector<double>> construct_interpolatedLine(auto const &pointA,
                                                                                          auto const &pointB,
                                                                                          double      ySteps,
                                                                                          double      xSteps) {
        // Weird calculation of total steps on the interpolated line ... probably reinventing the wheel here
        // TODO: Poosibly refactor according to some best practice on this
        int const totalSteps = std::max(0.0, ((ySteps)*Config::y_interpolationMultiplier) - 1.0) +
                               std::max(0.0, ((xSteps)*Config::x_interpolationMultiplier) - 1.0);
        double const yStepSize = (std::get<0>(pointB) - std::get<0>(pointA)) / static_cast<double>(totalSteps);
        double const xStepSize = (std::get<1>(pointB) - std::get<1>(pointA)) / static_cast<double>(totalSteps);

        std::pair<std::vector<double>, std::vector<double>> res;
        for (int i = 1; i < totalSteps; ++i) {
            res.first.push_back(std::get<0>(pointA) + (yStepSize * i));
            res.second.push_back(std::get<1>(pointA) + (xStepSize * i));
        }
        return res;
    }

public:
    template <typename X>
    requires std::is_arithmetic_v<typename X::value_type>
    static std::vector<std::string> drawPoints(size_t canvas_width, size_t canvas_height, X const &x_values,
                                               auto                                      viewOfValVariants,
                                               std::optional<std::vector<size_t>> const &catIDs_vec,
                                               std::array<Color_CVTS, 6>                 colorPalette) {

        BrailleDrawer bd(canvas_width, canvas_height,
                         catIDs_vec.has_value()
                             ? get_sortedAndUniqued(catIDs_vec.value()).size()
                             : std::ranges::count_if(viewOfValVariants, [](auto const &a) { return true; }),
                         colorPalette);

        auto [xMin, xMax] = std::ranges::minmax(x_values);
        auto [yMin, yMax] = compute_minMaxMulti(viewOfValVariants);
        double xStepSize  = (xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);


        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            auto y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            auto yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            auto x       = static_cast<size_t>(((xVal - xMin) / xStepSize)) / 2;
            auto xChrPos = static_cast<size_t>(((xVal - xMin) / xStepSize)) % 2;

            bd.m_canvasBraille[y][x] |= Config::braille_map[yChrPos][xChrPos];
            bd.m_pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]++;
        };

        for (size_t i = 0; auto const &one_yValCol : viewOfValVariants) {
            auto olSet = [&](auto const &oneCol) -> void {
                auto const &yValCol_data = oneCol.get();
                if constexpr (std::is_arithmetic_v<
                                  typename std::remove_reference_t<decltype(yValCol_data)>::value_type>) {
                    if (catIDs_vec.has_value()) {
                        for (size_t rowID = 0; rowID < x_values.size(); ++rowID) {
                            placePointOnCanvas(yValCol_data[rowID], x_values[rowID], catIDs_vec.value()[rowID]);
                        }
                    }
                    else {
                        for (size_t rowID = 0; rowID < x_values.size(); ++rowID) {
                            placePointOnCanvas(yValCol_data[rowID], x_values[rowID], i);
                        }
                    }
                    i++;
                }
            };
            std::visit(olSet, one_yValCol);
        }

        bd.compute_canvasColors();
        return bd.construct_outputPlotArea();
    }

    template <typename X>
    static std::vector<std::string> drawLines(size_t canvas_width, size_t canvas_height, X const &ts_values,
                                              auto viewOfValVariants, std::array<Color_CVTS, 6> colorPalette) {


        BrailleDrawer bd(canvas_width, canvas_height,
                         std::ranges::count_if(viewOfValVariants, [](auto const &a) { return true; }), colorPalette);


        auto [yMin, yMax] = compute_minMaxMulti_ALT(viewOfValVariants);
        double xStepSize  = 1;
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);


        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            size_t y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            size_t yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            size_t x       = static_cast<size_t>(xVal) / 2;
            size_t xChrPos = static_cast<size_t>(xVal) % 2;

            bd.m_canvasBraille[y][x]                                         |= Config::braille_map[yChrPos][xChrPos];
            bd.m_pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]  = 1;
        };

        std::vector<double> xValues;
        double              xScaler = ((canvas_width * 2) - 1) / static_cast<double>(ts_values.size() - 1);
        for (size_t i = 0; i < ts_values.size(); ++i) { xValues.push_back(i * xScaler); }

        auto viewCpy = viewOfValVariants;

        // Plot actual points on the bd canvas
        for (size_t i = 0; auto one_yValCol : viewOfValVariants) {
            auto olSet = [&](auto &oneCol) -> void {
                auto &yValCol_data = oneCol;
                if constexpr (std::is_arithmetic_v<
                                  std::ranges::range_value_t<std::remove_cvref_t<decltype(yValCol_data)>>>) {

                    for (auto const &[yVal, xVal] : std::views::zip(yValCol_data, xValues)) {
                        placePointOnCanvas(yVal, xVal, i);
                    }

                    // for (size_t rowID = 0; rowID < ts_values.size(); ++rowID) {
                    //     placePointOnCanvas(yValCol_data[rowID], xValues[rowID], i);
                    // }
                }
                else { assert(false); }
            };
            std::visit(olSet, one_yValCol);
            i++;
        }

        

        // Interpolate 'in between' every 2 points to actually get a line in the plot visually
        std::vector<size_t> interpolatedValues;
        for (size_t catID = 0; auto one_yValCol : viewCpy) {
            auto olSet = [&](auto &oneCol) -> void {
                auto &yValCol_data = oneCol;

                // yValCol needs to be arithmetic
                if constexpr (std::is_arithmetic_v<
                                  std::ranges::range_value_t<std::remove_cvref_t<decltype(yValCol_data)>>>) {
                    for (auto const &[pointA, pointB] :
                         (std::views::zip(yValCol_data, xValues) | std::views::pairwise)) {
                        auto intpLine = construct_interpolatedLine(
                            pointA, pointB, (std::abs((std::get<0>(pointB) - std::get<0>(pointA)) / yStepSize)),
                            std::abs(std::get<1>(pointB) - std::get<1>(pointA)));

                        for (size_t rowID = 0; rowID < intpLine.first.size(); ++rowID) {
                            placePointOnCanvas(intpLine.first.at(rowID), intpLine.second.at(rowID), catID);
                        }
                    }
                }
            };
            std::visit(olSet, one_yValCol);
            catID++;
        }

        bd.compute_canvasColors();
        return bd.construct_outputPlotArea();
    }
};

} // namespace detail
} // namespace terminal_plot
} // namespace incom