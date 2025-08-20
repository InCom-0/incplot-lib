#include <algorithm>
#include <expected>
#include <incplot/plot_structures.hpp>
#include <utility>


namespace incom {
namespace terminal_plot {
namespace plot_structures {

using incerr_c = incerr::incerr_code;
using enum Unexp_plotSpecs;
using enum Unexp_plotDrawer;

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

guess_retType BarV::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    if (dp.labelTS_colID.has_value()) {
        if (dp.labelTS_colID.value() >= ds.m_data.size()) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolNotFoundInData));
        }
    }
    else {
        // Suitable: 1) string_like AND 2) not selected as catCol AND 3) not selected as valCol
        auto filter2 = std::views::filter(
            std::views::enumerate(std::views::zip(ds.m_data, dp.m_colAssessments)), [&](auto const &ca) {
                return (std::get<0>(std::get<1>(ca)).colType == parsedVal_t::string_like) &&
                       (dp.cat_colID.has_value() ? std::get<0>(ca) != dp.cat_colID.value() : true) &&
                       std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == std::get<0>(ca); });
            });

        // Best == the one with the most "categories" ie. least number of identical strings in rows
        auto bestForLabels = std::ranges::max_element(filter2, [](auto const &lhs, auto const &rhs) {
            double const l_val = std::get<1>(std::get<1>(lhs)).categoryCount /
                                 static_cast<double>(std::get<0>(std::get<1>(lhs)).itemFlags.size());
            double const r_val = std::get<1>(std::get<1>(rhs)).categoryCount /
                                 static_cast<double>(std::get<0>(std::get<1>(rhs)).itemFlags.size());

            return l_val < r_val;
        });

        if (bestForLabels == filter2.end()) {
            return std::unexpected(incerr_c::make(GTSC_noStringLikeColumnForLabelsForBarPlot));
        }
        else { dp.labelTS_colID = std::get<0>(*bestForLabels); }
    }
    return dp_pr;
}
guess_retType BarV::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    if (dp.cat_colID.has_value()) {
        return std::unexpected(incerr_c::make(GCC_cantSpecifyCategoryForOtherThanScatter));
    }
    else { return dp_pr; }
    std::unreachable();
}
guess_retType BarV::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    if (dp.values_colIDs.size() > 1) { return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForBarV)); }

    // Verify that the selected column can actually be used for this plot
    else if (dp.values_colIDs.size() == 1) {
        return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForBarV));
    }

    // Select one column for this plot
    else { return std::unexpected(incerr_c::make(GVC_selectedMoreThan1YvalColForBarV)); }
    std::unreachable();
}
guess_retType BarV::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();
    // Width always need to be provided, otherwise the whole thing doesn't work
    if (not dp.targetWidth.has_value()) {
        // Is unknown ... defaulting to Config specified width
        if (not dp.availableWidth.has_value()) { dp.targetWidth = Config::default_targetWidth; }
        // Is known ... using it after scaling down a little
        else { dp.targetWidth = static_cast<size_t>(dp.availableWidth.value() * Config::scale_availablePlotWidth); }
    }

    if (dp.availableWidth.has_value()) {
        if (dp.targetWidth.value() > dp.availableWidth.value()) {
            return std::unexpected(incerr_c::make(GSZ_tarWidthLargerThanAvailableWidth));
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
guess_retType BarV::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();
    if (not dp.valAxesNames_bool.has_value()) { dp.valAxesNames_bool = false; }
    if (not dp.valAxesLabels_bool.has_value()) { dp.valAxesLabels_bool = false; }
    if (not dp.valAutoFormat_bool.has_value()) { dp.valAutoFormat_bool = true; }
    if (not dp.legend_bool.has_value()) { dp.legend_bool = false; }

    return dp_pr;
}
// ### END BAR V ###


// BAR VM

guess_retType BarVM::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::move(dp_pr), ds);
}
guess_retType BarVM::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::move(dp_pr), ds);
}
guess_retType BarVM::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::move(dp_pr), ds);
}
guess_retType BarVM::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::move(dp_pr), ds);
}

// ### END BAR VM ###


// SCATTER
guess_retType Scatter::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    // If TScol specified then verify if it is legit.
    if (dp.labelTS_colID.has_value()) {
        if (dp.labelTS_colID.value() >= ds.m_data.size()) {
            return std::unexpected(incerr_c::make(GTSC_selectedTScolNotFoundInData));
        }
        else if ((dp.cat_colID.has_value() ? dp.labelTS_colID.value() == dp.cat_colID.value() : false)) {
            return std::unexpected(incerr_c::make(GTSC_cantSelectTSColToBeTheSameAsCatCol));
        }
        else if (std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == dp.labelTS_colID.value(); })) {
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
guess_retType Scatter::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    auto useableCatCols_tpl = std::views::filter(
        std::views::zip(std::views::iota(0), ds.m_data, dp.m_colAssessments), [&](auto const &colType) {
            return (std::get<2>(colType).is_categoryLike &&
                    std::get<2>(colType).categoryCount <= Config::max_maxNumOfCategories);
        });

    // size_t useableCatCols_tpl_sz = std::ranges::count_if(useableCatCols_tpl, [](auto const &_) { return true; });
    size_t useableCatCols_tpl_sz = std::ranges::distance(useableCatCols_tpl.begin(), useableCatCols_tpl.end());

    // Impossible if there is no useable catCol or if the are more than 1 selected valCol
    if (useableCatCols_tpl_sz == 0) { return std::unexpected(incerr_c::make(GCC_specifiedCatColCantBeUsedAsCatCol)); }
    if (dp.values_colIDs.size() > 1) { return std::unexpected(incerr_c::make(GCC_cantSelectCatColAndMultipleYCols)); }

    // catCol specified need to verify that it is legit to use
    if (dp.cat_colID.has_value()) {
        bool calColID_found = std::ranges::find_if(useableCatCols_tpl, [&](auto const &tpl) {
                                  return std::get<0>(tpl) == dp.cat_colID.value();
                              }) != useableCatCols_tpl.end();

        // If the existing catColID cant be found then its wrong
        if (not calColID_found) { return std::unexpected(incerr_c::make(GCC_specifiedCatColCantBeUsedAsCatCol)); }
    }

    // catCol isn't specified ... select first one of the useable catCols
    else { dp.cat_colID = std::get<0>(useableCatCols_tpl.front()); }

    return dp_pr;
}
guess_retType Scatter::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::move(dp_pr), ds);
}
guess_retType Scatter::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::move(dp_pr), ds);
}

// ### END SCATTER ###


// MULTILINE
guess_retType Multiline::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

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
        else if (std::ranges::none_of(dp.values_colIDs, [&](auto const &a) { return a == dp.labelTS_colID.value(); })) {
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
guess_retType Multiline::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::move(dp_pr), ds);
}
guess_retType Multiline::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_sizes(std::move(dp_pr), ds);
}
guess_retType Multiline::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::move(dp_pr), ds);
}
// ### END MULTILINE ###


// BAR HM
guess_retType BarHM::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::move(dp_pr), ds);
}
guess_retType BarHM::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::move(dp_pr), ds);
}
guess_retType BarHM::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    if (dp.values_colIDs.size() < 1) { return std::unexpected(incerr_c::make(GVC_notEnoughSuitableYvalCols)); }
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    // Compute rowCount as it is required to evaluate whether there is enough 'width' to fit the plot
    auto compute_rowCount = [&](auto &var) -> size_t { return std::ranges::to<std::vector>(var).size(); };

    auto         dataViews = ds.get_filteredViewOfData(dp.values_colIDs, dp.filterFlags);
    size_t const rowCount  = std::visit(compute_rowCount, dataViews.front());

    size_t const desired_areaWidth =
        ((rowCount * dp.values_colIDs.size()) + (dp.values_colIDs.size() == 1 ? rowCount - 1 : 2 * rowCount));

    // +2ll for the 2 vertical axes
    long long const desired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_minWidth_legend_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value() && desired_targetWidth > dp.targetWidth) {
        return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
    }
    else if (dp.availableWidth.has_value() && desired_targetWidth > dp.availableWidth) {
        return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
    }
    // Neither targetWidth, neither availableWidth was specified ... fallback to some default value
    else {
        if (desired_targetWidth > Config::default_targetWidth) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    dp.targetWidth = desired_targetWidth;

    // Height
    if (dp.targetHeight.has_value() && dp.targetHeight.value() < Config::min_plotHeight) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }

    return dp_pr;
}
guess_retType BarHM::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::move(dp_pr), ds);
}
// ### END BAR HM ###


// BAR HS
guess_retType BarHS::guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TSCol(std::move(dp_pr), ds);
}
guess_retType BarHS::guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_catCol(std::move(dp_pr), ds);
}
guess_retType BarHS::guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds) {
    DesiredPlot &dp = dp_pr.first.get();

    // Compute rowCount as it is required to evaluate whether there is enough 'width' to fit the plot
    auto compute_rowCount = [&](auto &var) -> size_t { return std::ranges::to<std::vector>(var).size(); };

    auto         dataViews = ds.get_filteredViewOfData(dp.values_colIDs, dp.filterFlags);
    size_t const rowCount  = std::visit(compute_rowCount, dataViews.front());

    size_t const desired_areaWidth = (2 * rowCount) - 1;

    // +2ll for the 2 vertical axes
    long long const desired_targetWidth =
        desired_areaWidth + Config::ps_padLeft + Config::ps_padRight + 2ll + Config::max_valLabelSize +
        Config::axisLabels_padRight_vl +
        (dp.values_colIDs.size() > 1 ? Config::axisLabels_padLeft_vr + Config::axisLabels_minWidth_legend_vr
                                     : Config::axis_verName_width_vl);

    if (dp.targetWidth.has_value() && desired_targetWidth > dp.targetWidth) {
        return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
    }
    else if (dp.availableWidth.has_value() && desired_targetWidth > dp.availableWidth) {
        return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
    }
    // Neither targetWidth, neither availableWidth was specified ... fallback to some default value
    else {
        if (desired_targetWidth > Config::default_targetWidth) {
            return std::unexpected(incerr_c::make(GSZ_iferredTargetWidthLargerThanAvailableWidth));
        }
    }
    dp.targetWidth = desired_targetWidth;

    // Height
    if (dp.targetHeight.has_value() && dp.targetHeight.value() < Config::min_plotHeight) {
        return std::unexpected(incerr_c::make(GZS_heightTooSmall));
    }

    return dp_pr;
}
guess_retType BarHS::guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) {
    return BarV::guess_TFfeatures(std::move(dp_pr), ds);
}

// ### END BAR HS ###


} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom