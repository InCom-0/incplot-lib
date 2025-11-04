#pragma once

#include <algorithm>
#include <cassert>
#include <ranges>
#include <tuple>
#include <utility>
#include <variant>

#include <incstd/core/algos.hpp>
#include <private/detail.hpp>


namespace incom {
namespace terminal_plot {
namespace detail {
class BrailleDrawer {
private:
    std::vector<std::vector<std::u32string>>                                    m_canvasColors;
    std::vector<std::vector<char32_t>>                                          m_canvasBraille;
    std::vector<std::vector<std::array<std::array<std::vector<size_t>, 2>, 4>>> m_pointsCountPerPos_perColor;

    color_schemes::scheme16  m_colScheme;
    std::array<size_t, 12uz> m_colOrder;
    std::u32string           s_terminalDefault = detail::convert_u32u8(Config::term_setDefault);

    BrailleDrawer() {};
    BrailleDrawer(size_t canvas_width, size_t canvas_height, size_t numOf_categories,
                  color_schemes::scheme16 const &colScheme, std::array<size_t, 12uz> const &colOrder)
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
          m_colScheme{colScheme}, m_colOrder{colOrder} {};

    void compute_canvasColors() {
        ColorMixer cm(ColorMixer::compute_maxStepsPerColor(m_pointsCountPerPos_perColor), 3uz, m_colScheme.palette,
                      m_colOrder, m_colScheme.backgrond);
        for (size_t rowID = 0; rowID < m_pointsCountPerPos_perColor.size(); ++rowID) {
            for (size_t colID = 0; colID < m_pointsCountPerPos_perColor[rowID].size(); ++colID) {
                if (m_canvasBraille[rowID][colID] != Config::braille_blank) {
                    m_canvasColors[rowID][colID] = detail::convert_u32u8(
                        ANSI::get_fg(cm.compute_colorOfPosition(m_pointsCountPerPos_perColor[rowID][colID])));
                }
            }
        }
    }

    std::vector<std::string> construct_outputPlotArea() {
        std::vector<std::string> res;
        // Gotta start rows from the back because axes cross bottom left and 'row 0' is top left
        for (int rowID = m_canvasBraille.size() - 1; rowID > -1; --rowID) {
            std::u32string oneLine;
            std::u32string lastColor;
            for (size_t colID = 0; colID < m_canvasBraille.front().size(); ++colID) {
                if (m_canvasColors[rowID][colID].empty()) { oneLine.push_back(m_canvasBraille[rowID][colID]); }
                else {
                    if (lastColor != m_canvasColors[rowID][colID]) {
                        oneLine.append(m_canvasColors[rowID][colID]);
                        lastColor = m_canvasColors[rowID][colID];
                    }
                    oneLine.push_back(m_canvasBraille[rowID][colID]);
                    // oneLine.append(s_terminalDefault);
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
        // This seems a little wierd, but it actually works for the braille type unicode line drawing
        int const totalSteps = std::max(0.0, ((ySteps)*Config::y_interpolationMultiplier) - 1.0) +
                               std::max(0.0, ((xSteps)*Config::x_interpolationMultiplier) - 1.0);
        double const yStepSize = (std::get<0>(pointB) - std::get<0>(pointA)) / static_cast<double>(totalSteps);
        double const xStepSize = (std::get<1>(pointB) - std::get<1>(pointA)) / static_cast<double>(totalSteps);

        // Construct res and preallocate with zeros
        std::pair<std::vector<double>, std::vector<double>> res{
            std::vector<double>(totalSteps + 1, std::get<0>(pointA)),
            std::vector<double>(totalSteps + 1, std::get<1>(pointA))};

        for (int i = 1; i < totalSteps + 1; ++i) {
            res.first[i]  = res.first[i - 1] + yStepSize;
            res.second[i] = res.second[i - 1] + xStepSize;
        }
        return res;
    }

public:
    static std::vector<std::string> drawPoints(size_t canvas_width, size_t canvas_height, auto &view_labelTS_col,
                                               auto                                     &view_varValCols,
                                               std::optional<std::vector<size_t>> const &catIDs_vec,
                                               color_schemes::scheme16 colScheme, std::array<size_t, 12uz> colOrder) {

        BrailleDrawer bd(canvas_width, canvas_height,
                         catIDs_vec.has_value() ? get_sortedAndUniqued(catIDs_vec.value()).size()
                                                : std::ranges::size(view_varValCols),
                         colScheme, colOrder);

        auto [xMin, xMax] = incom::standard::algos::compute_minMaxMulti(view_labelTS_col.value());
        auto [yMin, yMax] = incom::standard::algos::compute_minMaxMulti(view_varValCols);
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
                                              auto &view_varValCols, color_schemes::scheme16 colScheme,
                                              std::array<size_t, 12uz> colOrder) {
        BrailleDrawer bd(canvas_width, canvas_height, std::ranges::distance(view_varValCols), colScheme, colOrder);

        auto [xMin, xMax] = incom::standard::algos::compute_minMaxMulti(view_labelTS_col);
        auto [yMin, yMax] = incom::standard::algos::compute_minMaxMulti(view_varValCols);
        double xStepSize  = (xMax - xMin) / ((static_cast<double>(canvas_width) * 2) - 1);
        double yStepSize  = (yMax - yMin) / ((static_cast<double>(canvas_height) * 4) - 1);

        auto placePointOnCanvas = [&](auto const &yVal, auto const &xVal, size_t const &groupID) {
            size_t y       = static_cast<size_t>(((yVal - yMin) / yStepSize)) / 4;
            size_t yChrPos = static_cast<size_t>(((yVal - yMin) / yStepSize)) % 4;

            size_t x       = static_cast<size_t>(((xVal - xMin) / xStepSize)) / 2;
            size_t xChrPos = static_cast<size_t>(((xVal - xMin) / xStepSize)) % 2;

            // We always set to 1 because we are not performing color shading in multiline plots
            bd.m_canvasBraille[y][x]                                         |= Config::braille_map[yChrPos][xChrPos];
            bd.m_pointsCountPerPos_perColor[y][x][yChrPos][xChrPos][groupID]  = 1;
        };

        auto createXValues = [](auto const &varVec) {
            std::vector<double> res;
            if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(varVec)>>>) {
                for (auto const &item :
                     varVec | std::views::transform([](auto &&item) { return static_cast<double>(item); })) {
                    res.push_back(item);
                }
                return res;
                // TODO: Once on GCC 15 (or supporting) use the below incantation instead as its simpler
                //  return std::vector<double>{std::from_range, varVec | std::views::transform([](auto &&item) {
                //                                                  return static_cast<double>(item);
                //                                              })};
            }
            else { return res; }
            std::unreachable();
        };
        std::vector<double> xValues = std::visit(createXValues, view_labelTS_col);

        // Interpolate 'in between' every 2 points to actually get a line in the plot visually
        for (size_t catID = 0; auto one_yValCol : view_varValCols) {
            auto olSet = [&](auto const &oneCol) -> void {
                // yValCol needs to be arithmetic
                if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(oneCol)>>>) {
                    size_t const sz = std::min(oneCol.size(), xValues.size());
                    for (size_t i = 0; (i + 1) < sz; ++i) {
                        auto intpLine = construct_interpolatedLine(
                            std::tie(oneCol.at(i), xValues.at(i)), std::tie(oneCol.at(i + 1), xValues.at(i + 1)),
                            (std::abs((oneCol.at(i + 1) - oneCol.at(i)) / yStepSize)),
                            std::abs(xValues.at(i + 1) - xValues.at(i)) / xStepSize);

                        for (auto const &[first, second] : std::ranges::views::zip(intpLine.first, intpLine.second)) {
                            placePointOnCanvas(first, second, catID);
                        }
                    }

                    // for (auto const &[pointA, pointB] : (std::views::zip(oneCol, xValues) | std::views::pairwise)) {
                    //     auto intpLine = construct_interpolatedLine(
                    //         pointA, pointB, (std::abs((std::get<0>(pointB) - std::get<0>(pointA)) / yStepSize)),
                    //         std::abs(std::get<1>(pointB) - std::get<1>(pointA)) / xStepSize);

                    //     for (auto const &[first, second] : std::ranges::views::zip(intpLine.first, intpLine.second)) {
                    //         placePointOnCanvas(first, second, catID);
                    //     }
                    // }
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