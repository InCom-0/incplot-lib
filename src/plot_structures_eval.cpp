#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <expected>
#include <iterator>
#include <limits>
#include <optional>
#include <ranges>
#include <utility>
#include <variant>

#include <incplot/plot_structures.hpp>
#include <incstd/core/algos.hpp>
#include <private/detail.hpp>


namespace incom {
namespace terminal_plot {
namespace plot_structures {

using incerr_c = incerr::incerr_code;
using enum Unexp_plotSpecs;
using enum Unexp_plotDrawer;

namespace detail {
std::vector<size_t> compute_groupByStdDevDistance(auto useableValCols_tplView, double const stdDev) {
    auto maxID = std::get<0>(*std::ranges::max_element(
        useableValCols_tplView, [](auto const &lhs, auto const &rhs) { return std::get<0>(lhs) < std::get<0>(rhs); }));

    std::vector vecOfGroups(maxID + 1, std::vector<size_t>{});
    for (auto lhs_iter = useableValCols_tplView.begin(); lhs_iter != useableValCols_tplView.end(); ++lhs_iter) {
        vecOfGroups[std::get<0>(*lhs_iter)].push_back(std::get<0>(*lhs_iter));
        for (auto rhs_iter = std::next(lhs_iter); rhs_iter != useableValCols_tplView.end(); ++rhs_iter) {
            auto const &lhsColAsse_ref = std::get<2>(*lhs_iter);
            auto const &rhsColAsse_ref = std::get<2>(*rhs_iter);

            double const meanDif = std::abs(lhsColAsse_ref.mean - rhsColAsse_ref.mean);
            if (meanDif < (stdDev * lhsColAsse_ref.standDev) && meanDif < (stdDev * rhsColAsse_ref.standDev)) {
                vecOfGroups[std::get<0>(*lhs_iter)].push_back(std::get<0>(*rhs_iter));
                vecOfGroups[std::get<0>(*rhs_iter)].push_back(std::get<0>(*lhs_iter));
            }
        }
    }

    auto res = *std::ranges::max_element(vecOfGroups,
                                         [](auto const &lhs, auto const &rhs) { return lhs.size() < rhs.size(); });
    return res;
}

std::expected<size_t, incerr_c> addColsUntil(std::vector<size_t> &out_dp_valCol, std::vector<size_t> useableValCols,
                                             size_t minAllowed, size_t addUntil_ifAvailable = 1) {
    auto filteredUseable = std::views::filter(useableValCols, [&](auto const &item) {
        // If useable valCol is NOT in dp_valCols then its ok
        if (not std::ranges::contains(out_dp_valCol, item)) { return true; }
        else { return false; }
    });

    auto   iter    = filteredUseable.begin();
    size_t counter = 0;
    while (out_dp_valCol.size() < minAllowed) {
        if (iter == filteredUseable.end()) { return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols)); }
        else {
            out_dp_valCol.push_back(*iter);
            ++iter, counter++;
        }
    }
    while (out_dp_valCol.size() < addUntil_ifAvailable) {
        if (iter == filteredUseable.end()) { break; }
        else {
            out_dp_valCol.push_back(*iter);
            ++iter, counter++;
        }
    }
    return 0uz;
}
} // namespace detail


using namespace incom::standard::algos;

// BASE
size_t Base::compute_lengthOfSelf() const {

    size_t lngth = pad_top + pad_bottom;

    // Horizontal top axis name and axis labels lines
    lngth += axisName_horTop_bool ? (axisName_horTop.size() + corner_topLeft.front().size() +
                                     corner_topRight.front().size() + pad_left + pad_right)
                                  : 0;
    for (size_t i = 0; i < labels_horTop.size(); ++i) {
        lngth += (labels_horTop.at(i).size() + corner_topLeft.at(i).size() + corner_topRight.at(i).size());
    }

    // First and last vertical labels + padding + vert axes names if present
    lngth += labels_verLeft.front().size() + labels_verRight.front().size() + labels_verLeft.back().size() +
             labels_verRight.back().size();

    // The 'area corner symbols'
    lngth += Config::areaCorner_tl.size() + Config::areaCorner_tr.size() + Config::areaCorner_bl.size() +
             Config::areaCorner_br.size();

    // All top and bottom axes
    for (int i = 0; i < areaWidth; i++) { lngth += (axis_horTop.at(i).size() + axis_horBottom.at(i).size()); }

    // Main plot area
    for (int i = 0; i < areaHeight; ++i) {
        lngth += labels_verLeft.at(i + 1).size() + labels_verRight.at(i + 1).size();
        lngth += axis_verLeft.at(i).size();
        lngth += axis_verRight.at(i).size();
        lngth += plotArea.at(i).size();
    }

    lngth += ((areaHeight + 2) * (pad_left + pad_right + (Config::axis_verName_width_vl * (axisName_verLeft_bool)) +
                                  (Config::axis_verName_width_vr * (axisName_verRight_bool))));

    // Horizontal bottom axis name and axis labels lines
    for (size_t i = 0; i < labels_horBottom.size(); ++i) {
        lngth += (labels_horBottom.at(i).size() + corner_bottomLeft.at(i).size() + corner_bottomRight.at(i).size());
    }
    lngth += labels_horBottom.size() * (pad_left + pad_right);

    lngth += axisName_horBottom_bool ? (axisName_horBottom.size() + corner_bottomLeft.back().size() +
                                        corner_bottomRight.back().size() + pad_left + pad_right)
                                     : 0;
    return lngth;
}
std::string Base::build_plotAsString() const {
    std::string result;
    result.reserve(compute_lengthOfSelf());

    // Add padding on top
    for (int i = 0; i < pad_top; ++i) { result.push_back('\n'); }

    // Build the heading lines of the plot
    if (axisName_horTop_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_topLeft.front());
        result.append(axisName_horTop);
        result.append(corner_topRight.front());
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }
    for (size_t i = 0; auto const &oneLabelLine : labels_horTop) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_topLeft.at(i));
        result.append(oneLabelLine);
        result.append(corner_topRight.at(i++));
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    // Build horizontal top axis line
    result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), Config::space));
    result.append(labels_verLeft.front());
    result.append(Config::color_Axes);
    result.append(areaCorner_tl);
    for (auto const &toAppend : axis_horTop) { result.append(toAppend); }
    result.append(Config::color_Axes);
    result.append(areaCorner_tr);
    result.append(Config::term_setDefault);
    result.append(labels_verRight.front());
    result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), Config::space));
    result.push_back('\n');

    // Add plot area lines
    for (long long i = 0; i < areaHeight; ++i) {
        result.append(std::string(pad_left, Config::space));
        if (axisName_verLeft_bool) { result.push_back(axisName_verLeft.at(i)); }
        result.append(std::string((Config::axis_verName_width_vl - 1) * axisName_verLeft_bool, Config::space));
        result.append(labels_verLeft.at(i + 1));
        result.append(axis_verLeft.at(i));
        result.append(plotArea.at(i));
        result.append(axis_verRight.at(i));
        result.append(labels_verRight.at(i + 1));
        result.append(std::string((Config::axis_verName_width_vr - 1) * axisName_verRight_bool, Config::space));
        if (axisName_verRight_bool) { result.push_back(axisName_verRight.at(i)); }
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    // Add horizontal bottom axis line
    result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), Config::space));
    result.append(labels_verLeft.back());
    result.append(Config::color_Axes);
    result.append(areaCorner_bl);
    for (auto const &toAppend : axis_horBottom) { result.append(toAppend); }
    result.append(Config::color_Axes);
    result.append(areaCorner_br);
    result.append(Config::term_setDefault);
    result.append(labels_verRight.back());
    result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), Config::space));
    result.push_back('\n');

    // Add the bottom lines of the plot

    for (size_t i = 0; auto const &oneLabelLine : labels_horBottom) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_bottomLeft.at(i));
        result.append(oneLabelLine);
        result.append(corner_bottomRight.at(i++));
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    if (axisName_horBottom_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_bottomLeft.back());
        result.append(axisName_horBottom);
        result.append(corner_bottomRight.back());
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    result.append(footer);

    // Add padding on bottom
    for (int i = 0; i < pad_bottom; ++i) { result.push_back('\n'); }
    return result;
}
// ### END BASE ###

// BAR V
guess_rt BarV::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.labelTS_colID.has_value()) {
        if (dp.labelTS_colID.value() >= ds.m_data.size()) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolNotFoundInData));
        }
    }
    else {
        // Suitable: 1) string_like AND 2) not selected as catCol AND 3) not selected as valCol
        auto filter2 = std::views::filter(
            std::views::enumerate(std::views::zip(ds.m_data, dp.m_colAssessments)), [&](auto const &ca) {
                bool const stringLike = std::get<0>(std::get<1>(ca)).colType == parsedVal_t::string_like;
                bool const tsLike     = std::get<1>(std::get<1>(ca)).is_timeSeriesLikeIndex;
                bool const notSelectedElsewhere =
                    (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                    std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });

                return (stringLike || tsLike) && notSelectedElsewhere;
            });

        // Best == the one with the most "categories" ie. least number of identical strings in rows
        auto bestForLabels = std::ranges::max_element(filter2, [](auto const &lhs, auto const &rhs) {
            bool const lhs_AllTheSame = std::get<1>(std::get<1>(lhs)).is_allValuesIdentical;
            bool const rhs_AllTheSame = std::get<1>(std::get<1>(rhs)).is_allValuesIdentical;
            bool const catCount_lhsSmaller =
                std::get<1>(std::get<1>(lhs)).categoryCount < std::get<1>(std::get<1>(rhs)).categoryCount;
            return (lhs_AllTheSame && (not rhs_AllTheSame)) || catCount_lhsSmaller;
        });

        if (bestForLabels == filter2.end()) {
            return std::unexpected(incerr_c::make(GTSC_noStringLikeColumnForLabelsForBarPlot));
        }
        else { dp.labelTS_colID = std::get<0>(*bestForLabels); }
    }
    return dp_pr;
}
guess_rt BarV::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.cat_colID.has_value()) {
        return std::unexpected(incerr_c::make(GCC_cantSpecifyCategoryForOtherThanScatter));
    }
    else { return dp_pr; }
    std::unreachable();
}
guess_rt BarV::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.values_colIDs.size() > 1) { return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForBarV)); }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                                 (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }
    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);

    if (auto retExp{detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 1)}) { return dp_pr; }
    else { return std::unexpected(retExp.error()); }
}
guess_rt BarV::compute_filterFlags(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    std::vector<size_t> colIDs;
    if (dp.labelTS_colID.has_value()) { colIDs.push_back(dp.labelTS_colID.value()); }
    if (dp.cat_colID.has_value()) { colIDs.push_back(dp.cat_colID.value()); }
    for (auto const &colID : dp.values_colIDs) { colIDs.push_back(colID); }

    dp.filterFlags = ds.compute_filterFlags(colIDs, dp.filter_outsideStdDev);
    return dp_pr;
}
guess_rt BarV::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();
    // Width always need to be provided, otherwise the whole thing doesn't work
    if (not dp.targetWidth.has_value()) {
        // Is unknown ... defaulting to Config specified width
        if (not dp.availableWidth.has_value()) { dp.targetWidth = Config::default_targetWidth; }
        // Is known ... using it after scaling down a little
        else { dp.targetWidth = static_cast<size_t>(dp.availableWidth.value() * Config::scale_availablePlotWidth); }
    }
    else {
        if (dp.availableWidth.has_value()) {
            if (dp.targetWidth.value() > dp.availableWidth.value()) {
                return std::unexpected(incerr_c::make(GSZ_tarWidthLargerThanAvailableWidth));
            }
        }
    }

    // Check for unreasonable width sizes
    if (dp.targetWidth.value() < Config::min_plotWidth) { return std::unexpected(incerr_c::make(GZS_widthTooSmall)); }
    else if (dp.targetWidth.value() > Config::max_plotWidth) {
        return std::unexpected(incerr_c::make(GZS_widthTooLarge));
    }

    // Height is generally inferred later in 'compute_descriptors' from computed actual 'areaWidth'
    // Impossible to print with height <5 under all circumstances
    if (dp.targetHeight.has_value() && dp.targetHeight.value() < Config::min_plotHeight) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }
    return dp_pr;
}
guess_rt BarV::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();
    if (not dp.valAxesNames_bool.has_value()) { dp.valAxesNames_bool = false; }
    if (not dp.valAxesLabels_bool.has_value()) { dp.valAxesLabels_bool = false; }
    if (not dp.valAutoFormat_bool.has_value()) { dp.valAutoFormat_bool = true; }
    if (not dp.legend_bool.has_value()) { dp.legend_bool = false; }

    return dp_pr;
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> BarV::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {
    return std::make_pair(dp_pr, 1uz);
}
// ### END BAR V ###


// BAR VM
guess_rt BarVM::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarVM::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarVM::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.values_colIDs.size() > Config::max_numOfValCols) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan6YvalColForBarXM));
    }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                                 (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }
    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    auto canAdd_prioritized =
        detail::compute_groupByStdDevDistance(useableValCols_tpl, Config::inColGroup_stdDevMultiplierAllowance);
    // auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);

    // Verify that the selected column can actually be used for this plot
    if (dp.values_colIDs.size() > 0) { return dp_pr; }
    else {
        if (auto retExp{detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 2, Config::max_numOfValCols)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
}
guess_rt BarVM::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarVM::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::forward<decltype(dp_pr)>(dp_pr), ds);
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> BarVM::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {
    return std::make_pair(dp_pr, (dp_pr.values_colIDs.size() > 1) * dp_pr.values_colIDs.size());
}
// ### END BAR VM ###


// SCATTER
guess_rt Scatter::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    // If TScol specified then verify if it is legit.
    if (dp.labelTS_colID.has_value()) {
        if (dp.labelTS_colID.value() >= ds.m_data.size()) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolNotFoundInData));
        }
        else if ((dp.cat_colID.has_value() ? dp.labelTS_colID.value() == dp.cat_colID.value() : false)) {
            return std::unexpected(incerr_c::make(GTSC_cantSelectTSColToBeTheSameAsCatCol));
        }
        else if (std::ranges::any_of(dp.values_colIDs, [&](auto const &a) { return a == dp.labelTS_colID.value(); })) {
            return std::unexpected(incerr_c::make(GTSC_cantSelectTSColToBeOneOfTheValCols));
        }
        return dp_pr;
    }

    // If TScol not specified then find a suitable one (the first one from the left)
    else {
        for (auto const &fvItem : std::views::filter(
                 std::views::enumerate(std::views::zip(ds.m_data, dp.m_colAssessments)), [&](auto const &ca) {
                     return (not std::get<1>(std::get<1>(ca)).is_timeSeriesLikeIndex) &&
                            (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                            (std::get<0>(std::get<1>(ca)).colType != parsedVal_t::string_like) &&
                            std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });
                 })) {

            dp.labelTS_colID = std::get<0>(fvItem);
            return dp_pr;
        }
        // If there are none (therefore none of the for loops execute at all) then return unexpected
        return std::unexpected(incerr_c::make(GTSC_noUnusedXvalColumnForScatter));
    }
    std::unreachable();
}
guess_rt Scatter::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    auto useableCatCols_tpl = std::views::filter(
        std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), [&](auto const &colType) {
            return (std::get<2>(colType).is_categoryLike &&
                    std::get<2>(colType).categoryCount <= Config::max_maxNumOfCategories);
        });

    // catCol specified need to verify that it is legit to use
    if (dp.cat_colID.has_value()) {
        bool calColID_found = std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                                  return std::get<0>(tpl) == dp.cat_colID.value();
                              }) != useableCatCols_tpl.end();
        // If the existing catColID cant be found then its wrong
        if (not calColID_found) { return std::unexpected(incerr_c::make(GCC_specifiedCatColCantBeUsedAsCatCol)); }
    }

    // catCol isn't specified ... try to select first one of the useable catCols if available
    else if (dp.values_colIDs.size() < 2 && std::ranges::distance(useableCatCols_tpl) > 0) {
        dp.cat_colID = std::get<0>(useableCatCols_tpl.front());
    }

    // If not available keep as is (catCol is not actually required)
    else {}

    return dp_pr;
}
guess_rt Scatter::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    // TODO: Consider potential use where the catCol isn't specified and cats are specified by individual columns
    // The above would require some very clever logic to find the 'most suitable' columns to select

    if (dp.cat_colID.has_value() && dp.values_colIDs.size() > Config::max_numOfValColsScatterCat) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForScatterCat));
    }
    else if (dp.values_colIDs.size() > Config::max_numOfValColsScatterNonCat) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan3YvalColForScatterNonCat));
    }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                                 (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }
    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    if (dp.cat_colID.has_value()) {
        auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);
        if (auto retExp{
                detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 1, Config::max_numOfValColsScatterCat)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
    else if (dp.values_colIDs.size() == 0) {
        auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);
        if (auto retExp{
                detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 1, Config::max_numOfValColsScatterNonCat)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
    else { return dp_pr; }
}
guess_rt Scatter::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt Scatter::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::forward<decltype(dp_pr)>(dp_pr), ds);
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> Scatter::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {
    size_t resPriority = dp_pr.cat_colID.has_value() ? std::numeric_limits<size_t>::max() / 2 : 0uz;

    return std::make_pair(dp_pr, resPriority);
}
// ### END SCATTER ###


// MULTILINE
guess_rt Multiline::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    // If TScol specified then verify if it is legit.
    if (dp.labelTS_colID.has_value()) {
        if (dp.labelTS_colID.value() >= ds.m_data.size()) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolNotFoundInData));
        }
        else if (not dp.m_colAssessments.at(dp.labelTS_colID.value()).is_timeSeriesLikeIndex) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolIsNotTimeSeriesLike));
        }
        else if ((dp.cat_colID.has_value() ? dp.labelTS_colID.value() == dp.cat_colID.value() : false)) {
            return std::unexpected(incerr_c::make(GTSC_cantSelectTSColToBeTheSameAsCatCol));
        }
        else if (std::ranges::any_of(dp.values_colIDs, [&](auto const &a) { return a == dp.labelTS_colID.value(); })) {
            return std::unexpected(incerr_c::make(GTSC_cantSelectTSColToBeOneOfTheValCols));
        }
        return dp_pr;
    }

    // If TScol not specified then find a suitable one (the first one from the left)
    else {
        for (auto const &fvItem : std::views::filter(std::views::enumerate(dp.m_colAssessments), [&](auto const &ca) {
                 return std::get<1>(ca).is_timeSeriesLikeIndex &&
                        (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                        std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });
             })) {

            dp.labelTS_colID = std::get<0>(fvItem);
            return dp_pr;
        }
        // If there are none (therefore none of the for loops execute at all) then return unexpected
        return std::unexpected(incerr_c::make(GTSC_noTimeSeriesLikeColumnForMultiline));
    }
    std::unreachable();
}
guess_rt Multiline::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt Multiline::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.values_colIDs.size() > Config::max_maxNumOfLinesInMultiline) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThanAllowedOfYvalColsForMultiline));
    }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }

    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    if (dp.values_colIDs.size() == 0) {
        auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);
        if (auto retExp{
                detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 1, Config::max_maxNumOfLinesInMultiline)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
    else { return dp_pr; }
}
guess_rt Multiline::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt Multiline::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::forward<decltype(dp_pr)>(dp_pr), ds);
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> Multiline::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {
    size_t rawRowCount =
        std::visit([](auto const &vec) { return vec.size(); }, ds.m_data.at(dp_pr.values_colIDs.front()).variant_data);
    long long const available_areaWidth =
        dp_pr.targetWidth.value() - Config::ps_padLeft - Config::ps_padRight - 2ll - Config::max_valLabelSize -
        Config::axisLabels_padRight_vl -
        (dp_pr.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_minWidth_legend_vr
                                        : Config::axis_verName_width_vl);

    // Highest priority if each step of the time series 'fits' into the are resolution nicely. If not => very low
    // priority
    return std::make_pair(dp_pr, rawRowCount <= (2 * available_areaWidth) ? std::numeric_limits<size_t>::max() : 0uz);
}
// ### END MULTILINE ###


// BAR HM
guess_rt BarHM::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarHM::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarHM::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.values_colIDs.size() > Config::max_numOfValCols) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan6YvalColForBarXM));
    }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                                 (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }
    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);

    // Verify that the selected column can actually be used for this plot
    if (dp.values_colIDs.size() > 0) { return dp_pr; }
    else {
        if (auto retExp{detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 2, Config::max_numOfValCols)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
}
guess_rt BarHM::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    // rowCount are the unfiltered row (filterFlag == 0u)
    size_t const rowCount = std::ranges::count(dp.filterFlags, 0u);

    size_t const desired_areaWidth =
        ((rowCount * dp.values_colIDs.size()) + (dp.values_colIDs.size() == 1 ? rowCount - 1 : 2 * rowCount));

    // +2ll for the 2 vertical axes
    size_t const minDesired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_minWidth_legend_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value()) {
        if (minDesired_targetWidth > dp.targetWidth.value()) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    else if (dp.availableWidth.has_value()) {
        if (minDesired_targetWidth > dp.availableWidth.value()) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    // Neither targetWidth, neither availableWidth was specified ... fallback to some default value
    else {
        if (minDesired_targetWidth > Config::default_targetWidth) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }

    size_t const maxDesired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_maxLength_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value()) { dp.targetWidth = std::min(dp.targetWidth.value(), maxDesired_targetWidth); }
    else if (dp.availableWidth.has_value()) {
        dp.targetWidth = std::min(dp.availableWidth.value(), maxDesired_targetWidth);
    }
    else { dp.targetWidth = std::min(Config::default_targetWidth, maxDesired_targetWidth); }

    // Height
    if (dp.targetHeight.has_value() &&
        (dp.targetHeight.value() < (Config::min_plotHeight + Config::axisLabels_maxHeight_hb - 1))) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }

    return dp_pr;
}
guess_rt BarHM::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::forward<decltype(dp_pr)>(dp_pr), ds);
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> BarHM::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {

    size_t resPriority = (std::numeric_limits<size_t>::max() / Config::max_numOfValCols) * dp_pr.values_colIDs.size();
    return std::make_pair(dp_pr, resPriority);
}
// ### END BAR HM ###


// BAR HS
guess_rt BarHS::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarHS::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::forward<decltype(dp_pr)>(dp_pr), ds);
}
guess_rt BarHS::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    if (dp.values_colIDs.size() > Config::max_numOfValCols) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan6YvalColForBarXM));
    }

    auto lam_filter = [&](auto const &tpl) {
        bool const arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                   std::get<1>(tpl).colType == parsedVal_t::double_like;
        // Not timeSeriesLike and Not categoryLike
        bool const notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                                 (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

        return (arithmeticCol && notExcluded && (not std::get<2>(tpl).is_categoryLike) &&
                (std::get<2>(tpl).is_allValuesNonNegative));
    };
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), lam_filter);

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (not std::ranges::contains(useableValCols_tpl, selColID, [](auto const &tpl) { return std::get<0>(tpl); })) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }
    // TODO: Do some sort of smarter sorting of potential yValCols ... for now turning it off
    auto lam_sorterComp = [&](auto const &lhs, auto const &rhs) {
        // return std::get<1>(lhs).categoryCount > std::get<1>(rhs).categoryCount;
        return false;
    };

    auto canAdd_prioritized = compute_filterSortedIDXs(lam_filter, lam_sorterComp, ds.m_data, dp.m_colAssessments);

    // Verify that the selected column can actually be used for this plot
    if (dp.values_colIDs.size() > 0) { return dp_pr; }
    else {
        if (auto retExp{detail::addColsUntil(dp.values_colIDs, canAdd_prioritized, 2, Config::max_numOfValCols)}) {
            return dp_pr;
        }
        else { return std::unexpected(retExp.error()); }
    }
}
guess_rt BarHS::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.get();

    // rowCount are the unfiltered row (filterFlag == 0u)
    size_t const rowCount = std::ranges::count(dp.filterFlags, 0u);

    size_t const desired_areaWidth = (2 * rowCount) - 1;

    // +2ll for the 2 vertical axes
    size_t const minDesired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_minWidth_legend_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value()) {
        if (minDesired_targetWidth > dp.targetWidth.value()) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    else if (dp.availableWidth.has_value()) {
        if (minDesired_targetWidth > dp.availableWidth.value()) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    // Neither targetWidth, neither availableWidth was specified ... fallback to some default value
    else {
        if (minDesired_targetWidth > Config::default_targetWidth) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }

    size_t const maxDesired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_maxLength_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value()) { dp.targetWidth = std::min(dp.targetWidth.value(), maxDesired_targetWidth); }
    else if (dp.availableWidth.has_value()) {
        dp.targetWidth = std::min(dp.availableWidth.value(), maxDesired_targetWidth);
    }
    else { dp.targetWidth = std::min(Config::default_targetWidth, maxDesired_targetWidth); }

    // Height
    if (dp.targetHeight.has_value() &&
        (dp.targetHeight.value() < (Config::min_plotHeight + Config::axisLabels_maxHeight_hb - 1))) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }

    return dp_pr;
}
guess_rt BarHS::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::forward<decltype(dp_pr)>(dp_pr), ds);
}

std::pair<incom::terminal_plot::DesiredPlot, size_t> BarHS::compute_priorityFactor(
    incom::terminal_plot::DesiredPlot &&dp_pr, DataStore const &ds) {

    size_t resPriority =
        (std::numeric_limits<size_t>::max() / (Config::max_numOfValCols + 2)) * dp_pr.values_colIDs.size();
    return std::make_pair(dp_pr, resPriority);
}
// ### END BAR HS ###


} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom