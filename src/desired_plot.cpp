#include <algorithm>
#include <functional>
#include <limits>
#include <optional>
#include <ranges>
#include <utility>

#include <incplot/desired_plot.hpp>
#include <incplot/plot_structures.hpp>
#include <private/concepts.hpp>
#include <private/detail.hpp>


namespace incom {
namespace terminal_plot {

using enum Unexp_plotSpecs;
using incerr_c = incerr::incerr_code;

// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::compute_colAssessments(DesiredPlot    &&dp,
                                                                                    DataStore const &ds) {

    auto c_catParams = [&](auto const &vecRef) -> void {
        auto vecCpy = vecRef;

        std::ranges::sort(vecCpy);
        auto   view_chunked = std::views::chunk_by(vecCpy, [](auto const &l, auto const &r) { return l == r; });
        size_t numOfChunks  = std::ranges::count_if(view_chunked, [](auto const &a) { return true; });

        // Save category count immediatelly (that is even if the column is not category like later)
        dp.m_colAssessments.back().categoryCount = numOfChunks;

        // Are all categories the same size?
        dp.m_colAssessments.back().is_categoriesSameSize = std::ranges::all_of(
            view_chunked, [&](auto const &chunk) { return (chunk.size() == view_chunked.front().size()); });

        // It is not categoryLike if there are more chunks than half the total num of elements
        // This is kind of arbitrary, but will work to filter out most
        // Also not a category when we have one chunk (that is column of identical values)
        if ((numOfChunks > (vecCpy.size() / 2)) || numOfChunks == 1) {
            dp.m_colAssessments.back().is_categoryLike = false;
        }
        // If any chunk has just one element, then it is not category (or the user should clean the data first)
        else if (std::ranges::any_of(view_chunked, [](auto const &oneChunk) { return oneChunk.size() < 2; })) {
            dp.m_colAssessments.back().is_categoryLike = false;
        }
        // If passed the above tests, then this could be a category column
        else { dp.m_colAssessments.back().is_categoryLike = true; }
    };

    auto is_srss = [&](auto const &vecRef) -> bool {
        auto const &vec = vecRef;
        if (vec.empty()) { return false; }

        auto const firstVal          = vec.at(0);
        size_t     firstValOccurence = 1;
        size_t     sequenceLength    = std::numeric_limits<size_t>::max();

        dp.m_colAssessments.back().is_sameRepeatingSubsequences_whole = false;
        size_t i_hlpr                                                 = 1;
        for (auto const &vecItem : std::views::drop(vec, 1)) {
            if (firstValOccurence == 1) {
                if (vecItem == firstVal) {
                    firstValOccurence++;
                    sequenceLength = i_hlpr;
                    i_hlpr         = 1;
                }
                else { i_hlpr++; }
            }
            else {
                if (vecItem == firstVal) {
                    if (i_hlpr != sequenceLength) { return false; }
                    firstValOccurence++;
                    i_hlpr = 1;
                }
                else if (vecItem != vec.at(i_hlpr++)) { return false; }
            }
        }
        if (i_hlpr == sequenceLength) { dp.m_colAssessments.back().is_sameRepeatingSubsequences_whole = true; }

        return (firstValOccurence != 1);
    };

    auto is_tsli = [&](auto const &vecRef) -> bool {
        auto const &vec = vecRef;
        if constexpr (std::is_arithmetic_v<typename std::remove_reference_t<decltype(vec)>::value_type>) {

            auto   vecOfChanges = std::views::pairwise(vec) | std::views::transform([](auto const &pr) {
                                    return (std::get<1>(pr) - std::get<0>(pr));
                                });
            double avg          = std::ranges::fold_left_first(vecOfChanges, std::plus()).value() /
                         static_cast<double>(vecOfChanges.size());
            auto allowHigh = avg + static_cast<decltype(avg)>(std::abs(avg * Config::timeSeriesIDX_allowanceUP));
            auto allowLow  = avg - static_cast<decltype(avg)>(std::abs(avg * Config::timeSeriesIDX_allowanceDOWN));

            return std::ranges::all_of(vecOfChanges,
                                       [&](auto const &chng) { return (allowLow < chng && chng < allowHigh); });
        }

        else { return false; }
    };

    for (auto const &oneCol : ds.m_data) {
        dp.m_colAssessments.push_back({0, false, false, false, false, false});

        std::visit(c_catParams, oneCol.variant_data);
        dp.m_colAssessments.back().is_sameRepeatingSubsequences = std::visit(is_srss, oneCol.variant_data);
        dp.m_colAssessments.back().is_timeSeriesLikeIndex       = std::visit(is_tsli, oneCol.variant_data);
    }
    return dp;
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                        DataStore const &ds) {
    if (dp.labelTS_colName.has_value()) {
        auto it =
            std::ranges::find_if(ds.m_data, [&](auto const &col) { return col.name == dp.labelTS_colName.value(); });
        if (it == ds.m_data.end()) { return std::unexpected(incerr_c::make(TNCII_colByNameNotExist)); }
        else if (not dp.labelTS_colID.has_value()) { dp.labelTS_colID = it - ds.m_data.begin(); }
        else { return std::unexpected(incerr_c::make(TNCII_colByNameNotExist)); }

        dp.labelTS_colName = std::nullopt;
    }

    for (auto const &v_colName : dp.values_colNames) {
        auto it = std::ranges::find_if(ds.m_data, [&](auto const &col) { return col.name == v_colName; });
        if (it == ds.m_data.end()) { return std::unexpected(incerr_c::make(TNCII_colByNameNotExist)); }

        auto it2 = std::ranges::find(dp.values_colIDs, it - ds.m_data.begin());
        if (it2 == dp.values_colIDs.end()) { dp.values_colIDs.push_back(it - ds.m_data.begin()); }
    }
    dp.values_colNames.clear();
    return dp;
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_plotType(DesiredPlot &&dp, DataStore const &ds) {
    if (dp.plot_type_name.has_value()) { return dp; }

    // Helpers
    size_t useableValCols_count =
        std::ranges::count_if(std::views::zip(ds.m_data, dp.m_colAssessments), [&](auto const &pr) {
            bool arithmeticCol = std::get<0>(pr).colType == parsedVal_t::signed_like ||
                                 std::get<0>(pr).colType == parsedVal_t::double_like;

            // Not timeSeriesLike and Not categoryLike
            bool notExcluded = not std::get<1>(pr).is_timeSeriesLikeIndex && not std::get<1>(pr).is_categoryLike;

            return (arithmeticCol && notExcluded);
        });

    size_t tsLikeIndexCols_count =
        std::ranges::count_if(dp.m_colAssessments, [](auto const &colPars) { return colPars.is_timeSeriesLikeIndex; });

    // ACTUAL DEICISIOM MAKING
    // Can't plot anything without at least 1 value column
    if (useableValCols_count == 0) { return std::unexpected(incerr_c::make(GPT_zeroUseableValueColumns)); }

    // labelTS_colID was specified
    else if (dp.labelTS_colID.has_value()) {
        if (ds.m_data.at(dp.labelTS_colID.value()).colType == parsedVal_t::string_like) {
            if (dp.values_colIDs.size() < 2) { dp.plot_type_name = detail::TypeToString<plot_structures::BarV>(); }
            else { dp.plot_type_name = detail::TypeToString<plot_structures::BarHM>(); }
        }
        else {
            // If labelTSCol is value then select based on whether its is 'timeSeriesLike' or not
            if (dp.m_colAssessments.at(dp.labelTS_colID.value()).is_timeSeriesLikeIndex == true) {
                dp.plot_type_name = detail::TypeToString<plot_structures::Multiline>();
            }
            else { dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>(); }
        }
    }

    // labelTS_colID is not specified
    else {
        if (tsLikeIndexCols_count != 0) { dp.plot_type_name = detail::TypeToString<plot_structures::Multiline>(); }
        else if (dp.values_colIDs.size() == 0) {
            // No TSlikeCol and one useable val col
            if (useableValCols_count == 1) { dp.plot_type_name = detail::TypeToString<plot_structures::BarV>(); }
            // No TSlikeCol and more than one useable val col
            else { dp.plot_type_name = detail::TypeToString<plot_structures::BarHM>(); }
        }
        else if (dp.values_colIDs.size() == 1) { dp.plot_type_name = detail::TypeToString<plot_structures::BarV>(); }
        else { dp.plot_type_name = detail::TypeToString<plot_structures::BarHM>(); }
    }
    return dp;
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_TSCol(DesiredPlot &&dp, DataStore const &ds) {
    if (dp.labelTS_colID.has_value()) { return dp; }

    if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
        for (auto const &fvItem : std::views::filter(std::views::enumerate(dp.m_colAssessments), [&](auto const &ca) {
                 return std::get<1>(ca).is_timeSeriesLikeIndex &&
                        (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                        std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });
             })) {

            dp.labelTS_colID = std::get<0>(fvItem);
            return dp;
        }
        // If there are none (therefore none of the for loops execute at all) then return unexpected
        return std::unexpected(incerr_c::make(GTSC_noTimeSeriesLikeColumnForMultiline));
    }
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
        for (auto const &fvItem : std::views::filter(
                 std::views::enumerate(std::views::zip(ds.m_data, dp.m_colAssessments)), [&](auto const &ca) {
                     return (not std::get<1>(std::get<1>(ca)).is_timeSeriesLikeIndex) &&
                            (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                            (std::get<0>(std::get<1>(ca)).colType != parsedVal_t::string_like) &&
                            std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });
                 })) {

            dp.labelTS_colID = std::get<0>(fvItem);
            return dp;
        }
        // If there are none (therefore none of the for loops execute at all) then return unexpected
        return std::unexpected(incerr_c::make(GTSC_noUnusedXvalColumnForScatter));
    }

    else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::BarVM>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::BarHM>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::BarHS>()) {
        for (auto const &fvItem : std::views::filter(std::views::enumerate(ds.m_data), [](auto const &ct) {
                 return std::get<1>(ct).colType == parsedVal_t::string_like;
             })) {
            dp.labelTS_colID = std::get<0>(fvItem);
            return dp;
        }
        // If there are none (therefore none of the for loops execute at all) then return unexpected
        return std::unexpected(incerr_c::make(GTSC_noStringLikeColumnForLabelsForBarPlot));
    }
    return std::unexpected(incerr_c::make(GTSC_unreachableCodeReached));
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_catCol(DesiredPlot &&dp, DataStore const &ds) {
    auto useableCatCols_tpl = std::views::filter(
        std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), [&](auto const &colType) {
            return (std::get<2>(colType).is_categoryLike &&
                    std::get<2>(colType).categoryCount <= Config::max_maxNumOfCategories);
        });
    size_t useableCatCols_tpl_sz = std::ranges::count_if(useableCatCols_tpl, [](auto const &_) { return true; });
    // BAR PLOTS
    if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarV>() ||
        dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarVM>() ||
        dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarHM>() ||
        dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarHS>()) {
        if (dp.cat_colID.has_value()) { return std::unexpected(incerr_c::make(GCC_cantSpecifyCategoryForBarPlots)); }
        else { return dp; }
    }
    // SCATTER PLOT
    else if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::Scatter>()) {
        if (dp.cat_colID.has_value()) {
            if (useableCatCols_tpl_sz == 0) {
                return std::unexpected(incerr_c::make(GCC_specifiedCatColCantBeUsedAsCatCol));
            }
            bool calColID_found = std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                                      return std::get<0>(tpl) == dp.cat_colID.value();
                                  }) != useableCatCols_tpl.end();

            // If the existing catColID cant be found then its wrong
            if (not calColID_found) { return std::unexpected(incerr_c::make(GCC_specifiedCatColCantBeUsedAsCatCol)); }

            // TODO: Fix this somehow the logic seems wrong mixing a different case here?
            if (dp.values_colIDs.size() > 1) {
                return std::unexpected(incerr_c::make(GCC_cantSelectCatColAndMultipleYCols));
            }
            else { return dp; }
        }
        else {
            // Possibly add catCol if at most 1 yVal cols and suitable catCol is available
            if (dp.values_colIDs.size() <= 1 && useableCatCols_tpl_sz > 0) {
                dp.cat_colID = std::get<0>(useableCatCols_tpl.front());
            }
            return dp;
        }
    }
    // MULTILINE PLOT
    else if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::Multiline>()) {
        if (dp.cat_colID.has_value()) {
            return std::unexpected(incerr_c::make(GCC_categoryColumnIsNotAllowedForMultiline, "Custom Test"sv));
        }
        else { return dp; }
    }

    return dp;
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_valueCols(DesiredPlot &&dp, DataStore const &ds) {
    // TODO: Inference for multibar plots
    auto useableValCols_tpl =
        std::views::filter(std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), [&](auto const &tpl) {
            bool arithmeticCol = std::get<1>(tpl).colType == parsedVal_t::signed_like ||
                                 std::get<1>(tpl).colType == parsedVal_t::double_like;

            // Not timeSeriesLike and Not categoryLike
            bool notExcluded = (dp.cat_colID.has_value() ? (std::get<0>(tpl) != dp.cat_colID.value()) : true) &&
                               (dp.labelTS_colID.has_value() ? (std::get<0>(tpl) != dp.labelTS_colID.value()) : true);

            return (arithmeticCol && notExcluded);
        });

    // Check if selected cols are actually useable
    for (auto const &selColID : dp.values_colIDs) {
        if (std::ranges::find_if(useableValCols_tpl, [&](auto const &tpl) { return std::get<0>(tpl) == selColID; }) ==
            useableValCols_tpl.end()) {
            return std::unexpected(incerr_c::make(GVC_selectYvalColIsUnuseable));
        }
    }

    auto addValColsUntil = [&](size_t minAllowed, size_t maxAllowed = 1) -> std::expected<size_t, incerr_c> {
        auto getAnotherValColID = [&]() -> std::expected<size_t, incerr_c> {
            for (auto const &tpl : useableValCols_tpl) {
                if (std::ranges::find(dp.values_colIDs, std::get<0>(tpl)) == dp.values_colIDs.end()) {
                    return std::get<0>(tpl);
                }
            }
            // Cannot find another one
            return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
        };
        while (dp.values_colIDs.size() < minAllowed) {
            auto expID = getAnotherValColID();
            if (expID.has_value()) { dp.values_colIDs.push_back(expID.value()); }
            else { return std::unexpected(expID.error()); }
        }
        while (dp.values_colIDs.size() < maxAllowed) {
            auto expID = getAnotherValColID();
            if (expID.has_value()) { dp.values_colIDs.push_back(expID.value()); }
            else { break; }
        }
        return 0uz;
    };

    // BAR PLOTS
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        if (dp.values_colIDs.size() > 1) {
            return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForBarV));
        }
        else if (not addValColsUntil(1).has_value()) {
            return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
        }
    }
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarVM>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::BarHM>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::BarHS>()) {
        if (dp.values_colIDs.size() > 6) {
            return std::unexpected(incerr_c::make(GVC_selectedMoreThan6YvalColForBarXM));
        }
        else if (dp.values_colIDs.size() == 1) {}
        else if (not addValColsUntil(2).has_value()) {
            return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
        }
    }

    // SCATTER PLOT
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
        if (dp.values_colIDs.size() > Config::max_numOfValCols) {
            return std::unexpected(incerr_c::make(GVC_selectedMoreThanMaxNumOfYvalCols));
        }
        else if (dp.cat_colID.has_value()) {
            if (dp.values_colIDs.size() <= 1) {
                if (not addValColsUntil(1, 1).has_value()) {
                    return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
                }
            }
            else { return std::unexpected(incerr_c::make(GVC_cantSelectCatColAndMultipleYCols)); }
        }
        else {
            if (dp.values_colIDs.size() < 1) {
                if (not addValColsUntil(1, Config::max_numOfValCols).has_value()) {
                    return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
                }
            }
        }
    }

    // MULTILINE PLOT
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
        if (dp.values_colIDs.size() > Config::max_maxNumOfLinesInMultiline) {
            return std::unexpected(incerr_c::make(GVC_selectedMoreThanAllowedOfYvalColsForMultiline));
        }
        else if (dp.values_colIDs.size() == 0) {
            if (not addValColsUntil(1, 3).has_value()) {
                return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols));
            }
        }
    }

    return dp;
}
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_sizes(DesiredPlot &&dp, DataStore const &ds) {
    // TODO: Inference for multibar plots

    // Width always need to be provided, otherwise the whole thing doesn't work
    if (not dp.targetWidth.has_value()) {
        // Is unknown ... defaulting to Config specified width
        if (not dp.availableWidth.has_value()) { dp.targetWidth = Config::default_targetWidth; }
        // Is known ... using it after scaling down a little
        else { dp.targetWidth = static_cast<size_t>(dp.availableWidth.value() * Config::scale_availablePlotWidth); }
    }

    // Check for unreasonable width sizes
    if (dp.targetWidth.value() < Config::min_plotWidth) { return std::unexpected(incerr_c::make(GZS_widthTooSmall)); }
    else if (dp.targetWidth.value() > Config::max_plotWidth) {
        return std::unexpected(incerr_c::make(GZS_widthTooLarge));
    }


    // Height is generally inferred later in 'compute_descriptors' from computed actual 'areaWidth'
    if (not dp.targetHeight.has_value()) {
        // if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {}
        // else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {}
        // else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {}
        // else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarVM>()) {}
        // else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarHM>()) {}
        // else { dp.targetHeight = dp.targetWidth.value() / 2; }
    }

    // Impossible to print with height <5 under all circumstances
    if (dp.targetHeight.has_value() && dp.targetHeight.value() < Config::min_plotHeight) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }
    return dp;
}
// This method doesn't do anything useful (yet)
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_TFfeatures(DesiredPlot &&dp, DataStore const &ds) {
    if (not dp.valAxesNames_bool.has_value()) { dp.valAxesNames_bool = false; }
    if (not dp.valAxesLabels_bool.has_value()) { dp.valAxesLabels_bool = false; }
    if (not dp.valAutoFormat_bool.has_value()) { dp.valAutoFormat_bool = true; }
    if (not dp.legend_bool.has_value()) { dp.legend_bool = false; }

    return dp;
}

std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::compute_filterFlags(DesiredPlot    &&dp,
                                                                                 DataStore const &ds) {
    std::vector<size_t> colIDs;
    if (dp.labelTS_colID.has_value()) { colIDs.push_back(dp.labelTS_colID.value()); }
    if (dp.cat_colID.has_value()) { colIDs.push_back(dp.cat_colID.value()); }
    for (auto const &colID : dp.values_colIDs) { colIDs.push_back(colID); }

    dp.filterFlags = ds.compute_filterFlags(colIDs, dp.filter_outsideStdDev);

    return dp;
};
std::expected<DesiredPlot, incerr::incerr_code> DesiredPlot::guess_missingParams(this DesiredPlot &&self,
                                                                                 DataStore const   &ds) {
    // Guesses the missing 'desired parameters' and returns a DesiredPlot with those filled in
    // Variation on a 'builder pattern'
    // Normally called 'in place' on 'DesiredPlot' instance constructed as rvalue
    // If impossible to guess or otherwise the user desires something impossible returns Err_plotSpecs.
    return DesiredPlot::compute_colAssessments(std::forward<decltype(self)>(self), ds)
        .and_then(std::bind_back(DesiredPlot::transform_namedColsIntoIDs, ds))
        .and_then(std::bind_back(DesiredPlot::guess_plotType, ds))
        .and_then(std::bind_back(DesiredPlot::guess_TSCol, ds))
        .and_then(std::bind_back(DesiredPlot::guess_catCol, ds))
        .and_then(std::bind_back(DesiredPlot::guess_valueCols, ds))
        .and_then(std::bind_back(DesiredPlot::guess_sizes, ds))
        .and_then(std::bind_back(DesiredPlot::guess_TFfeatures, ds))
        .and_then(std::bind_back(DesiredPlot::compute_filterFlags, ds));
}


} // namespace terminal_plot
} // namespace incom
