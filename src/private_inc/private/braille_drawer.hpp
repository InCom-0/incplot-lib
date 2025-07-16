#pragma once

#include <algorithm>
#include <cassert>
#include <private/detail.hpp>
#include <ranges>
#include <utility>
#include <variant>


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

          m_pointsCountPerPos_perColor(std::vector(
              canvas_height,
              std::vector(
                  canvas_width,
                  std::array<std::array<std::vector<size_t>, 2>, 4>{
                      {{std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz)},
                       {std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz)},
                       {std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz)},
                       {std::vector<size_t>(numOf_categories, 0uz), std::vector<size_t>(numOf_categories, 0uz)}}}))),
          m_colorPallete{detail::convert_u32u8(TermColors::get_basicColor(colorPalette[0])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[1])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[2])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[3])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[4])),
                         detail::convert_u32u8(TermColors::get_basicColor(colorPalette[5]))} {};

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
    static std::vector<std::string> drawPoints(size_t canvas_width, size_t canvas_height, auto &view_labelTS_col,
                                               auto                                     &view_varValCols,
                                               std::optional<std::vector<size_t>> const &catIDs_vec,
                                               std::array<Color_CVTS, 6>                 colorPalette) {

        BrailleDrawer bd(canvas_width, canvas_height,
                         catIDs_vec.has_value()
                             ? get_sortedAndUniqued(catIDs_vec.value()).size()
                             : std::ranges::count_if(view_varValCols, [](auto const &a) { return true; }),
                         colorPalette);

        auto [xMin, xMax] = compute_minMaxMulti(view_labelTS_col.value());
        auto [yMin, yMax] = compute_minMaxMulti(view_varValCols);
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

        for (size_t i = 0; auto &one_yValCol : view_varValCols) {
            auto olSet = [&](auto &oneCol) -> void {
                if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(oneCol)>>>) {
                    if (catIDs_vec.has_value()) {
                        auto vis = [&](auto &lts_col) {
                            if constexpr (std::is_arithmetic_v<
                                              std::ranges::range_value_t<std::remove_cvref_t<decltype(lts_col)>>>) {
                                for (size_t rowID = 0; auto const &[yVal, xVal] : std::views::zip(oneCol, lts_col)) {
                                    placePointOnCanvas(yVal, xVal, catIDs_vec.value()[rowID]);
                                    rowID++;
                                }
                            }
                            else { assert(false); }
                        };
                        std::visit(vis, view_labelTS_col.value());
                    }
                    else {
                        auto vis = [&](auto &lts_col) {
                            if constexpr (std::is_arithmetic_v<
                                              std::ranges::range_value_t<std::remove_cvref_t<decltype(lts_col)>>>) {
                                for (auto const &[yVal, xVal] : std::views::zip(oneCol, lts_col)) {
                                    placePointOnCanvas(yVal, xVal, i);
                                }
                            }
                            else { assert(false); }
                        };
                        std::visit(vis, view_labelTS_col.value());
                    }
                    i++;
                }
                else { assert(false); }
            };
            std::visit(olSet, one_yValCol);
        }

        bd.compute_canvasColors();
        return bd.construct_outputPlotArea();
    }

    static std::vector<std::string> drawLines(size_t canvas_width, size_t canvas_height, auto &view_labelTS_col,
                                              auto &view_varValCols, std::array<Color_CVTS, 6> colorPalette) {
        BrailleDrawer bd(canvas_width, canvas_height,
                         std::ranges::count_if(view_varValCols, [](auto const &a) { return true; }), colorPalette);

        auto [xMin, xMax] = compute_minMaxMulti(view_labelTS_col);
        auto [yMin, yMax] = compute_minMaxMulti(view_varValCols);
        double xStepSize  = (xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);

        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            size_t y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            size_t yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            size_t x       = static_cast<size_t>(((xVal - xMin) / xStepSize)) / 2;
            size_t xChrPos = static_cast<size_t>(((xVal - xMin) / xStepSize)) % 2;

            bd.m_canvasBraille[y][x]                                         |= Config::braille_map[yChrPos][xChrPos];
            bd.m_pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]  = 1;
        };

        // size_t labelTS_col_sz = 0;
        // auto   vis            = [&](auto &&var) {
        //     for (auto const &_ : var) { labelTS_col_sz++; }
        // };
        // std::visit(vis, view_labelTS_col);


        auto createXValues = [](auto const &varVec) {
            std::vector<double> res;
            if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(varVec)>>>) {
                for (auto const &item :
                     varVec | std::views::transform([](auto &&item) { return static_cast<double>(item); })) {
                    res.push_back(item);
                }
                return res;
                //TODO: Once on GCC 15 (or supporting) use the below incantation instead as its simpler
                // return std::vector<double>{std::from_range, varVec | std::views::transform([](auto &&item) {
                //                                                 return static_cast<double>(item);
                //                                             })};
            }
            else { return res; }
            std::unreachable();
        };
        std::vector<double> xValues = std::visit(createXValues, view_labelTS_col);

        // Plot actual points on the bd canvas
        for (size_t i = 0; auto one_yValCol : view_varValCols) {
            auto olSet = [&](auto &oneCol) -> void {
                if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(oneCol)>>>) {
                    for (auto const &[yVal, xVal] : std::views::zip(oneCol, xValues)) {
                        placePointOnCanvas(yVal, xVal, i);
                    }
                }
                else { assert(false); }
            };
            std::visit(olSet, one_yValCol);
            i++;
        }


        // Interpolate 'in between' every 2 points to actually get a line in the plot visually
        std::vector<size_t> interpolatedValues;
        for (size_t catID = 0; auto one_yValCol : view_varValCols) {
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