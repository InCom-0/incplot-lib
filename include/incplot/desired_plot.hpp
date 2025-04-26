#pragma once

#include <algorithm>
#include <expected>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <incplot/detail/concepts.hpp>
#include <incplot/detail/misc.hpp>


namespace incom {
namespace terminal_plot {
// FORWARD DELCARATIONS
namespace plot_structures {
class Base;
class BarV;
class BarH;
class Multiline;
class Scatter;
} // namespace plot_structures


// FORWARD DELCARATIONS --- END

// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'
class DesiredPlot {
private:
    struct ColumnParams {
        size_t categoryCount;

        bool is_categoriesSameSize;
        bool is_categoryLike;
        bool is_sameRepeatingSubsequences;
        bool is_sameRepeatingSubsequences_whole;
        bool is_timeSeriesLikeIndex;
    };

    std::vector<ColumnParams> m_colAssessments;

    std::vector<int> colsUsed;

    // BUILDING METHODS
    static std::expected<DesiredPlot, Unexp_plotSpecs> compute_colAssessments(DesiredPlot &&dp, DataStore const &ds) {

        auto c_catParams = [&](auto const &vecRef) -> void {
            auto vecCpy = vecRef.get();

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
            auto const &vec = vecRef.get();
            if (vec.empty()) { return false; }

            auto const firstVal          = vec.at(0);
            size_t     firstValOccurence = 1;
            size_t     sequenceLength    = SIZE_MAX;

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
            auto const &vec = vecRef.get();
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

        for (auto const &oneCol : ds.vec_colVariants) {
            dp.m_colAssessments.push_back({0, false, false, false, false, false});

            std::visit(c_catParams, oneCol);
            dp.m_colAssessments.back().is_sameRepeatingSubsequences = std::visit(is_srss, oneCol);
            dp.m_colAssessments.back().is_timeSeriesLikeIndex       = std::visit(is_tsli, oneCol);
        }
        dp.colsUsed = std::vector(dp.m_colAssessments.size(), 0);
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                  DataStore const &ds) {
        if (dp.labelTS_colName.has_value()) {
            auto it = std::ranges::find(ds.colNames, dp.labelTS_colName.value());
            if (it == ds.colNames.end()) { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }
            else if (not dp.labelTS_colID.has_value()) { dp.labelTS_colID = it - ds.colNames.begin(); }
            else if ((it - ds.colNames.begin()) == dp.labelTS_colID.value()) { dp.labelTS_colName = std::nullopt; }
            else { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }
        }

        for (auto const &v_colName : dp.values_colNames) {
            auto it = std::ranges::find(ds.colNames, v_colName);
            if (it == ds.colNames.end()) { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }

            auto it2 = std::ranges::find(dp.values_colIDs, it - ds.colNames.begin());
            if (it2 == dp.values_colIDs.end()) { dp.values_colIDs.push_back(it2 - dp.values_colIDs.begin()); }

            dp.values_colNames.clear();
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_plotType(DesiredPlot &&dp, DataStore const &ds) {
        if (dp.plot_type_name.has_value()) { return dp; }

        // Helpers
        size_t useableValCols_count =
            std::ranges::count_if(std::views::zip(ds.colTypes, dp.m_colAssessments), [&](auto const &colType) {
                bool arithmeticCol = std::get<0>(colType).first == parsedVal_t::signed_like ||
                                     std::get<0>(colType).first == parsedVal_t::double_like;

                // Not timeSeriesLike and Not categoryLike
                bool notExcluded =
                    not std::get<1>(colType).is_timeSeriesLikeIndex && not std::get<1>(colType).is_categoryLike;

                return (arithmeticCol && notExcluded);
            });

        size_t tsLikeIndexCols_count = std::ranges::count_if(
            dp.m_colAssessments, [](auto const &colPars) { return colPars.is_timeSeriesLikeIndex; });

        size_t labelCols_sz =
            std::ranges::count_if(std::views::zip(ds.colTypes, dp.m_colAssessments), [&](auto const &colType) {
                // Must be string AND not categoryLike
                return (std::get<0>(colType).first == parsedVal_t::string_like &&
                        not std::get<1>(colType).is_categoryLike);
            });

        // ACTUAL DEICISIOM MAKING
        // Can't plot anything without at least 1 value column
        if (useableValCols_count == 0) { return std::unexpected(Unexp_plotSpecs::valCols); }

        // labelTS_colID was specified
        else if (dp.labelTS_colID.has_value()) {
            if (ds.colTypes.at(dp.labelTS_colID.value()).first == parsedVal_t::string_like) {
                if (dp.values_colIDs.size() < 2) { dp.plot_type_name = detail::TypeToString<plot_structures::BarV>(); }
                // More than 1 value cols is impossible with labelTS col being string
                else { return std::unexpected(Unexp_plotSpecs::plotType); }
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
                else { dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>(); }
            }
            else if (dp.values_colIDs.size() == 1) {
                dp.plot_type_name = detail::TypeToString<plot_structures::BarV>();
            }
            else { dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>(); }
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_TSCol(DesiredPlot &&dp, DataStore const &ds) {
        if (not dp.labelTS_colID.has_value()) {
            if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
                for (auto const &fvItem :
                     std::views::filter(std::views::enumerate(dp.m_colAssessments),
                                        [](auto const &ca) { return std::get<1>(ca).is_timeSeriesLikeIndex; })) {

                    dp.labelTS_colID = std::get<0>(fvItem);
                    return dp;
                }
                // If there are none (therefore the loop doesn't execute at all) then return unexpected
                return std::unexpected(Unexp_plotSpecs::TScol);
            }
            else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
                for (auto const &fvItem : std::views::filter(std::views::enumerate(ds.colTypes), [](auto const &ct) {
                         return std::get<1>(ct).first == parsedVal_t::string_like;
                     })) {
                    dp.labelTS_colID = std::get<0>(fvItem);
                    return dp;
                }
                // If there are none (therefore the loop doesn't execute at all) then return unexpected
                return std::unexpected(Unexp_plotSpecs::TScol);
            }
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_catCol(DesiredPlot &&dp, DataStore const &ds) {
        auto useableCatCols_tpl = std::views::filter(
            std::views::zip(std::views::iota(0), ds.colTypes, dp.m_colAssessments), [&](auto const &colType) {
                return (std::get<2>(colType).is_categoryLike &&
                        std::get<2>(colType).categoryCount <= Config::max_maxNumOfCategories);
            });
        size_t useableCatCols_tpl_sz = std::ranges::count_if(useableCatCols_tpl, [](auto const &_) { return true; });
        // BAR PLOTS
        if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarV>()) {
            if (dp.cat_colID.has_value()) { return std::unexpected(Unexp_plotSpecs::catCol); }
            else { return dp; }
        }
        else if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::BarH>()) {
            if (useableCatCols_tpl_sz == 0) { return std::unexpected(Unexp_plotSpecs::catCol); }
            else if (dp.cat_colID.has_value()) {
                // If the existing catColID can be found in useable CatCols then all OK.
                if (std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                        return std::get<0>(tpl) == dp.cat_colID.value();
                    }) != useableCatCols_tpl.end()) {
                    return dp;
                }
                else { return std::unexpected(Unexp_plotSpecs::catCol); }
            }
            else {
                // The first catCol available is taken to be the category
                dp.cat_colID = std::get<0>(useableCatCols_tpl.front());
                return dp;
            }
        }
        // SCATTER PLOT
        else if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::Scatter>()) {
            if (dp.cat_colID.has_value()) {
                if (useableCatCols_tpl_sz == 0) { return std::unexpected(Unexp_plotSpecs::catCol); }
                bool calColID_found = std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                                          return std::get<0>(tpl) == dp.cat_colID.value();
                                      }) != useableCatCols_tpl.end();
                // If the existing catColID can be found in useable CatCols then all OK.
                if (calColID_found && dp.values_colIDs.size() <= Config::max_numOfValCols) { return dp; }
                else { return std::unexpected(Unexp_plotSpecs::catCol); }
            }
            else {
                if (dp.values_colIDs.size() <= Config::max_numOfValCols && useableCatCols_tpl_sz > 0) {
                    dp.cat_colID = std::get<0>(useableCatCols_tpl.front());
                }
                return dp;
            }
        }
        // MULTILINE PLOT
        else if (dp.plot_type_name.value() == detail::TypeToString<plot_structures::Multiline>()) {
            if (dp.cat_colID.has_value()) {
                if (useableCatCols_tpl_sz == 0) { return std::unexpected(Unexp_plotSpecs::catCol); }
                bool calColID_found = std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                                          return std::get<0>(tpl) == dp.cat_colID.value();
                                      }) != useableCatCols_tpl.end();
                // If the existing catColID can be found in useable CatCols then all OK.
                if (calColID_found && dp.values_colIDs.size() <= (Config::max_numOfValCols - 1)) { return dp; }
                else { return std::unexpected(Unexp_plotSpecs::catCol); }
            }
            else {
                if (dp.values_colIDs.size() <= (Config::max_numOfValCols - 1) && useableCatCols_tpl_sz > 0) {
                    dp.cat_colID = std::get<0>(useableCatCols_tpl.front());
                }
                return dp;
            }
        }

        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_valueCols(DesiredPlot &&dp, DataStore const &ds) {
        auto useableValCols_tpl = std::views::filter(
            std::views::zip(std::views::iota(0), ds.colTypes, dp.m_colAssessments), [&](auto const &colType) {
                bool arithmeticCol = std::get<1>(colType).first == parsedVal_t::signed_like ||
                                     std::get<1>(colType).first == parsedVal_t::double_like;

                // Not timeSeriesLike and Not categoryLike
                bool notExcluded =
                    (dp.cat_colID.has_value() ? (std::get<0>(colType) != dp.cat_colID.value()) : true) &&
                    (dp.labelTS_colID.has_value() ? (std::get<0>(colType) != dp.labelTS_colID.value()) : true);

                return (arithmeticCol && notExcluded);
            });

        // Check if selected cols are actually useable
        for (auto const &selColID : dp.values_colIDs) {
            if (std::ranges::find_if(useableValCols_tpl, [&](auto const &tpl) { return true; }) ==
                useableValCols_tpl.end()) {
                return std::unexpected(Unexp_plotSpecs::selectedUnuseableCols);
            }
        }

        auto addValColsUntil = [&](size_t minAllowed, size_t maxAllowed = 1) -> std::expected<size_t, Unexp_plotSpecs> {
            auto getAnotherValColID = [&]() -> std::expected<size_t, Unexp_plotSpecs> {
                for (auto const &tpl : useableValCols_tpl) {
                    if (std::ranges::find(dp.values_colIDs, std::get<0>(tpl)) == dp.values_colIDs.end()) {
                        return std::get<0>(tpl);
                    }
                }
                // Cannot find another one
                return std::unexpected(Unexp_plotSpecs::guessValCols);
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
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
            if (dp.values_colIDs.size() > Config::max_numOfValCols) {
                return std::unexpected(Unexp_plotSpecs::valCols);
            }
            else if (dp.cat_colID.has_value()) {
                if (dp.values_colIDs.size() <= 2) {
                    if (not addValColsUntil(2, 2).has_value()) {
                        return std::unexpected(Unexp_plotSpecs::guessValCols);
                    }
                }
            }
            else {
                if (dp.values_colIDs.size() < 2) {
                    if (not addValColsUntil(2, Config::max_numOfValCols).has_value()) {
                        return std::unexpected(Unexp_plotSpecs::guessValCols);
                    }
                }
            }
        }

        // MULTILINE PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (dp.values_colIDs.size() == 0) {
                if (not addValColsUntil(1, 3).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
            }
        }

        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_sizes(DesiredPlot &&dp, DataStore const &ds) {

        // Width always need to be provided, otherwise the whole thing doesn't work
        if (not dp.targetWidth.has_value()) { dp.targetWidth = 64; }
        else if (dp.targetWidth.value() < 24 || dp.targetWidth.value() > 256) {
            return std::unexpected(Unexp_plotSpecs::tarWidth);
        }

        // Height is generally inferred later in 'compute_descriptors' from computed actual 'areaWidth'
        if (not dp.targetHeight.has_value()) {
            if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {}
            else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {}
            else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {}
            else { dp.targetHeight = dp.targetWidth.value() / 2; }
        }

        // Impossible to print with height <5 under all circumstances
        if (dp.targetHeight.has_value() && dp.targetHeight.value() < 5) {
            return std::unexpected(Unexp_plotSpecs::tarWidth);
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_TFfeatures(DesiredPlot &&dp, DataStore const &ds) {
        if (not dp.valAxesNames_bool.has_value()) { dp.valAxesNames_bool = false; }
        if (not dp.valAxesLabels_bool.has_value()) { dp.valAxesLabels_bool = false; }
        if (not dp.valAutoFormat_bool.has_value()) { dp.valAutoFormat_bool = true; }
        if (not dp.legend_bool.has_value()) { dp.legend_bool = false; }

        return dp;
    }


public:
    std::optional<std::string> plot_type_name;

    // Category column ID
    std::optional<size_t>      cat_colID; // ID in colTypes
    std::optional<std::string> cat_colName;

    // Label for use in timeseries
    std::optional<size_t>      labelTS_colID; // ID in colTypes
    std::optional<std::string> labelTS_colName;

    // TODO: Make both 'values_' into std::optional as well to keep the logic the same for all here
    std::vector<size_t>      values_colIDs; // IDs in colTypes
    std::vector<std::string> values_colNames;

    std::optional<size_t> targetHeight;
    std::optional<size_t> targetWidth;

    std::array<Color_CVTS, 6> color_basePalette;

    std::optional<bool> valAxesNames_bool;
    std::optional<bool> valAxesLabels_bool;
    std::optional<bool> valAutoFormat_bool;
    std::optional<bool> legend_bool;

    // TODO: Provide some compile time programmatic way to set the default sizes here
    struct DP_CtorStruct {
        std::optional<size_t>      tar_width      = std::nullopt;
        std::optional<size_t>      tar_height     = std::nullopt;
        std::optional<std::string> plot_type_name = std::nullopt;
        std::optional<size_t>      lts_colID      = std::nullopt;
        std::vector<size_t>        v_colIDs       = {};
        std::optional<size_t>      c_colID        = std::nullopt;
        std::array<Color_CVTS, 6>  colors         = {
            Config::color_Vals1_enum, Config::color_Vals2_enum, Config::color_Vals3_enum,
            Config::color_Vals4_enum, Config::color_Vals5_enum, Config::color_Vals6_enum,
        };
        std::optional<std::string> lts_colName = std::nullopt;
        std::vector<std::string>   v_colNames  = {};
        std::optional<std::string> c_colName   = std::nullopt;
    };

    DesiredPlot(DP_CtorStruct &&dp_struct)
        : targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          plot_type_name(std::move(dp_struct.plot_type_name)), labelTS_colID(std::move(dp_struct.lts_colID)),
          values_colIDs(std::move(dp_struct.v_colIDs)), cat_colID(std::move(dp_struct.c_colID)),
          color_basePalette(std::move(dp_struct.colors)), labelTS_colName(std::move(dp_struct.lts_colName)),
          values_colNames(std::move(dp_struct.v_colNames)), cat_colName(std::move(dp_struct.c_colName)) {}
    DesiredPlot(DP_CtorStruct const &dp_struct)
        : targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          plot_type_name(dp_struct.plot_type_name), labelTS_colID(dp_struct.lts_colID),
          values_colIDs(dp_struct.v_colIDs), cat_colID(dp_struct.c_colID), color_basePalette(dp_struct.colors),
          labelTS_colName(dp_struct.lts_colName), values_colNames(dp_struct.v_colNames),
          cat_colName(dp_struct.c_colName) {}

    // Create a new copy and guess_missingParams on it.
    std::expected<DesiredPlot, Unexp_plotSpecs> build_guessedParamsCPY(this DesiredPlot &self, DataStore const &ds) {
        return DesiredPlot(self).guess_missingParams(ds);
    }

    // Guesses the missing 'desired parameters' and returns a DesiredPlot with those filled in
    // Variation on a 'builder pattern'
    // Normally called 'in place' on 'DesiredPlot' instance constructed as rvalue
    // If impossible to guess or otherwise the user desires something impossible returns Err_plotSpecs.
    std::expected<DesiredPlot, Unexp_plotSpecs> guess_missingParams(this DesiredPlot &&self, DataStore const &ds) {
        // TODO: Could use std::bind for these ... had some trouble with that ... maybe return to it later.
        // Still can't quite figure it out ...  std::bind_back doesn't seem to cooperate with and_then ...

        auto tnciids = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::transform_namedColsIntoIDs(std::forward<decltype(dp)>(dp), ds);
        };
        auto gpt = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_plotType(std::forward<decltype(dp)>(dp), ds);
        };
        auto gtsc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_TSCol(std::forward<decltype(dp)>(dp), ds);
        };
        auto gcc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_catCol(std::forward<decltype(dp)>(dp), ds);
        };
        auto gvc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_valueCols(std::forward<decltype(dp)>(dp), ds);
        };
        auto gsz = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_sizes(std::forward<decltype(dp)>(dp), ds);
        };
        auto gtff = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_TFfeatures(std::forward<decltype(dp)>(dp), ds);
        };

        return DesiredPlot::compute_colAssessments(std::forward<decltype(self)>(self), ds)
            .and_then(tnciids)
            .and_then(gpt)
            .and_then(gtsc)
            .and_then(gcc)
            .and_then(gvc)
            .and_then(gsz)
            .and_then(gtff);
    }
};


} // namespace terminal_plot
} // namespace incom
