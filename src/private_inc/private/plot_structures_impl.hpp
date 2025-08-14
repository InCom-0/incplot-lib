#pragma once

#include "incplot/config.hpp"
#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <expected>
#include <iterator>
#include <limits>
#include <ranges>
#include <string>
#include <type_traits>
#include <variant>

#include <incplot/plot_structures.hpp>
#include <private/braille_drawer.hpp>
#include <vector>


namespace incom {
namespace terminal_plot {
namespace plot_structures {
using incerr_c = incerr::incerr_code;
using enum Unexp_plotDrawer;

// BASE
auto Base::build_self(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Can only build it from rvalue ...
    if constexpr (std::is_lvalue_reference_v<decltype(self)>) { static_assert(false); }

    using self_t = std::remove_cvref_t<decltype(self)>;

    return std::move(self)
        .initialize_data_views()

        .and_then(&self_t::template compute_descriptors<self_t>)
        .and_then(&self_t::template validate_descriptors<self_t>)

        .and_then(&self_t::template compute_axisName_vl<self_t>)
        .and_then(&self_t::template compute_axisName_vr<self_t>)

        .and_then(&self_t::template compute_labels_vl<self_t>)
        .and_then(&self_t::template compute_labels_vr<self_t>)

        .and_then(&self_t::template compute_axis_vl<self_t>)
        .and_then(&self_t::template compute_axis_vr<self_t>)

        .and_then(&self_t::template compute_axis_ht<self_t>)
        .and_then(&self_t::template compute_axisName_ht<self_t>)
        .and_then(&self_t::template compute_labels_ht<self_t>)

        .and_then(&self_t::template compute_axis_hb<self_t>)
        .and_then(&self_t::template compute_axisName_hb<self_t>)
        .and_then(&self_t::template compute_labels_hb<self_t>)

        .and_then(&self_t::template compute_corner_tl<self_t>)
        .and_then(&self_t::template compute_corner_bl<self_t>)
        .and_then(&self_t::template compute_corner_br<self_t>)
        .and_then(&self_t::template compute_corner_tr<self_t>)
        .and_then(&self_t::template compute_areaCorners<self_t>)

        .and_then(&self_t::template compute_plot_area<self_t>)
        .and_then(&self_t::template compute_footer<self_t>);
}
inline size_t Base::compute_lengthOfSelf() const {

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
inline std::string Base::build_plotAsString() const {
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

auto BarV::initialize_data_views(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    if (self.dp.labelTS_colID.has_value()) {
        auto dataView = self.ds.get_filteredViewOfData(self.dp.labelTS_colID.value(), self.dp.filterFlags);

        auto create_LOC_storage = [&](auto &var) { self.labelTS_data = std::ranges::to<std::vector>(var); };
        std::visit(create_LOC_storage, dataView);
    }
    else { return std::unexpected(incerr_c::make(INI_labelTS_colID_isNull)); }

    if (self.dp.cat_colID.has_value()) {
        auto dataView = self.ds.get_filteredViewOfData(self.dp.cat_colID.value(), self.dp.filterFlags);

        auto create_LOC_storage = [&](auto &var) { self.cat_data = std::ranges::to<std::vector>(var); };
        std::visit(create_LOC_storage, dataView);
    }

    if (self.dp.values_colIDs.size() == 0) { return std::unexpected(incerr_c::make(INI_values_colIDs_isEmpty)); }
    else {
        auto dataViews = self.ds.get_filteredViewOfData(self.dp.values_colIDs, self.dp.filterFlags);
        for (auto &viewRef : dataViews) {
            auto create_LOC_storage = [&](auto &var) { self.values_data.push_back(std::ranges::to<std::vector>(var)); };
            std::visit(create_LOC_storage, viewRef);
        }
        // Compute row count once so it is not required ad-hoc
        self.data_rowCount = std::visit([&](auto &&varVec) { return varVec.size(); }, self.values_data.at(0));
        if (self.data_rowCount == 0) { return std::unexpected(incerr_c::make(INI_values_rowCount_isZero)); }
    }
    return self;
}

auto BarV::compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // VERTICAL LEFT LABELS SIZE
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        // auto &labelColVarRef = self.ds.m_data.at(self.dp.labelTS_colID.value()).variant_data;

        auto olset = [](auto &var) -> size_t {
            if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
                return std::ranges::max(
                    std::views::transform(var, [](auto const &a) { return detail::strlen_utf8(a); }));
            }
            else { return Config::max_sizeOfValueLabels; }
        };

        auto const maxLabelSize = std::visit(olset, self.labelTS_data.value());

        self.labels_verLeftWidth =
            std::min(Config::axisLabels_maxLength_vl,
                     std::min(maxLabelSize,
                              static_cast<size_t>((self.dp.targetWidth.value() - self.pad_left - self.pad_right) / 4)));
    }
    else { self.labels_verLeftWidth = Config::max_valLabelSize; }

    // VERTICAL RIGHT LABELS SIZE
    // Will be used as 'legend' for some types of Plots
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
        // catCol is specified meaning the legend will be uniqued values from that column
        if (self.dp.cat_colID.has_value()) {
            auto create_catIDs_vec = [&](auto &vec) -> std::vector<std::string> {
                auto sortedUniqued = detail::get_sortedAndUniqued(vec);
                if constexpr (std::same_as<typename decltype(sortedUniqued)::value_type, std::string>) {
                    return sortedUniqued;
                }
                else {
                    std::vector<std::string> res;
                    for (auto const &suItem : sortedUniqued) { res.push_back(std::to_string(suItem)); }
                    return res;
                }
            };

            auto   catIDs_vec = std::visit(create_catIDs_vec, self.cat_data.value());
            size_t maxSize =
                std::ranges::max(std::views::transform(catIDs_vec, [](auto const &a) { return a.size(); }));
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
        // Else if more than one YvalCols, legend will be column names of those YvalCols
        else if (self.dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : self.dp.values_colIDs) {
                maxSize = std::max(maxSize, self.ds.m_data.at(colID).name.size());
            }
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
        else { self.labels_verRightWidth = 0; }
    }
    else if (self.dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
        if (self.dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : self.dp.values_colIDs) {
                maxSize = std::max(maxSize, self.ds.m_data.at(colID).name.size());
            }
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
    }
    else { self.labels_verRightWidth = 0; }

    // VERTICAL AXES NAMES ... LEFT always, RIGHT never
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.axisName_verLeft_bool  = false;
        self.axisName_verRight_bool = false;
    }
    else if (self.dp.values_colIDs.size() > 1) {
        self.axisName_verLeft_bool  = false;
        self.axisName_verRight_bool = false;
    }
    else {
        self.axisName_verLeft_bool  = true;
        self.axisName_verRight_bool = false;
    }

    // PLOT AREA
    // Plot area width (-2 is for the 2 vertical axes positions)
    self.areaWidth = self.dp.targetWidth.value() - self.pad_left -
                     (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2 -
                     self.labels_verRightWidth - (Config::axis_verName_width_vr * self.axisName_verRight_bool) -
                     self.pad_right;
    if (self.areaWidth < static_cast<long long>(Config::min_areaWidth)) {
        return std::unexpected(incerr_c::make(C_DSC_areaWidth_insufficient));
    }

    // LABELS AND AXIS NAME HOR BOTTOM
    // TODO: Proper assessment for Multiline
    self.axisName_horBottom_bool = true;


    // Labels and axis name top ... probably nothing so keeping 0 size
    // ...

    // Plot area height (-2 is for the 2 horizontal axes positions)
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {


        self.areaHeight = std::visit([](auto &a) { return std::ranges::count_if(a, [](auto &&a2) { return true; }); },
                                     self.values_data.at(0));
    }
    else if (not self.dp.targetHeight.has_value()) {
        if (self.dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            //
            if (not self.dp.availableWidth.has_value() || not self.dp.availableHeight.has_value()) {
                self.areaHeight = self.areaWidth / Config::default_areaWidth2Height_ratio;
            }
            else {
                self.areaHeight = self.areaWidth * (static_cast<double>(self.dp.availableHeight.value()) /
                                                    self.dp.availableWidth.value());
            }
        }
        else {
            if (not self.dp.availableWidth.has_value() || not self.dp.availableHeight.has_value()) {
                self.areaHeight = self.areaWidth / 3;
            }
            else {
                self.areaHeight = self.areaWidth * (static_cast<double>(self.dp.availableHeight.value()) /
                                                    self.dp.availableWidth.value());
            }
        }
    }
    else {
        self.areaHeight = static_cast<long long>(self.dp.targetHeight.value()) - self.pad_top -
                          self.axisName_horTop_bool - self.labels_horTop.size() - 2ll - self.labels_horBottom.size() -
                          self.axisName_horBottom_bool - self.pad_bottom;
    }
    if (self.areaHeight < static_cast<long long>(Config::min_areaHeight)) {
        return std::unexpected(incerr_c::make(C_DSC_areaHeight_insufficient));
    }

    // Axes steps
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.axis_verLeftSteps = self.areaHeight;
    }
    else { self.axis_verLeftSteps = detail::guess_stepsOnVerAxis(self.areaHeight); }


    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarVM>()) {
        self.axis_horBottomSteps = self.areaWidth;
    }
    else { self.axis_horBottomSteps = detail::guess_stepsOnHorAxis(self.areaWidth); }

    // Top and Right axes steps keeping as-is

    return self;
}

auto BarV::compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_verLeft_bool) {
        if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.labelTS_colID.value()).name, self.areaHeight);
        }
        else {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.values_colIDs.at(0)).name, self.areaHeight);
        }
    }
    return self;
}
auto BarV::compute_axisName_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}

auto BarV::compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Empty label at the top
    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    auto olset = [&](auto &var) -> void {
        for (auto const &rawLabel : var) {
            if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
                self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth));
            }
            else {
                self.labels_verLeft.push_back(
                    detail::trim2Size_leading(detail::format_toMax5length(rawLabel), self.labels_verLeftWidth));
            }

            for (size_t i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                self.labels_verLeft.back().push_back(Config::space);
            }
        }
    };

    std::visit(olset, self.labelTS_data.value());

    // Empty label at the bottom
    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    return (self);
}
auto BarV::compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    for (int i = 0; i < (self.areaHeight + 2); ++i) { self.labels_verRight.push_back(""); }
    return self;
}

auto BarV::compute_axis_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.axis_verLeft =
            detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l, self.areaHeight, self.areaHeight);
    }
    // All else should have vl axis ticks according to numeric values
    else {
        auto tmpAxis = detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l, self.axis_verLeftSteps,
                                                     self.areaHeight);
        std::ranges::reverse(tmpAxis);
        self.axis_verLeft = std::move(tmpAxis);
    }
    return self;
}
auto BarV::compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = std::vector(self.areaHeight, std::string(" "));
    return self;
}


auto BarV::compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_horTop = std::vector(self.areaWidth, std::string(" "));
    return self;
}
auto BarV::compute_axisName_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}
auto BarV::compute_labels_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}


auto BarV::compute_axis_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Axis with ticks is contructed according to num of 'steps' which is the num of ticks and the areaWidth
    self.axis_horBottom = detail::create_tickMarkedAxis(Config::axisFiller_b, Config::axisTick_b,
                                                        self.axis_horBottomSteps, self.areaWidth);
    return self;
}
auto BarV::compute_axisName_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Name of the FIRST value column
    self.axisName_horBottom =
        detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.values_colIDs.at(0)).name, self.areaWidth);
    return self;
}
auto BarV::compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computeLabels = [&](auto &var) -> void {
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            assert(false);
        }
        else {
            size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
            auto const [minV, maxV] = std::ranges::minmax(var);
            auto   stepSize         = (maxV - minV) / static_cast<double>((self.areaWidth));
            size_t placedChars      = 0;

            self.labels_horBottom.push_back(std::string());
            self.labels_horBottom.back().append(Config::color_Axes);

            // Construct the [0:0] point label
            std::string tempStr = detail::format_toMax5length(minV);
            self.labels_horBottom.back().append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);

            // Construct the tick labels
            for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
                while (placedChars < (i * (fillerSize + 1) + fillerSize)) {
                    self.labels_horBottom.back().push_back(Config::space);
                    placedChars++;
                }
                tempStr = detail::format_toMax5length(minV + ((i * (fillerSize + 1) + fillerSize) * stepSize));
                self.labels_horBottom.back().append(tempStr);
                placedChars += detail::strlen_utf8(tempStr);
            }

            // Construct the [0:end] point label
            tempStr = detail::format_toMax5length(maxV + stepSize);
            for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
                self.labels_horBottom.back().push_back(Config::space);
            }
            self.labels_horBottom.back().append(tempStr);
            self.labels_horBottom.back().append(Config::term_setDefault);
        }
    };

    std::visit(computeLabels, self.values_data.at(0));
    return self;
}


// All corners are simply empty as default ... but can possibly be used for something later if overriden in
// derived
auto BarV::compute_corner_tl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horTop_bool) {
        self.corner_topLeft.push_back(std::string(self.labels_verLeftWidth +
                                                      (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                      Config::axisLabels_padRight_vl,
                                                  Config::space));
    }
    for (size_t i = 0; i < self.labels_horTop.size(); ++i) {
        self.corner_topLeft.push_back(std::string(self.labels_verLeftWidth +
                                                      (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                      Config::axisLabels_padRight_vl,
                                                  Config::space));
    }

    return self;
}
auto BarV::compute_corner_bl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horBottom_bool) {
        self.corner_bottomLeft.push_back(std::string(self.labels_verLeftWidth +
                                                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                         Config::axisLabels_padRight_vl,
                                                     Config::space));
    }
    for (size_t i = 0; i < self.labels_horBottom.size(); ++i) {
        self.corner_bottomLeft.push_back(std::string(self.labels_verLeftWidth +
                                                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                         Config::axisLabels_padRight_vl,
                                                     Config::space));
    }
    return self;
}
auto BarV::compute_corner_tr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horTop_bool) {
        self.corner_topRight.push_back(std::string(self.labels_verRightWidth +
                                                       (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                                       Config::axisLabels_padLeft_vr,
                                                   Config::space));
    }
    for (size_t i = 0; i < self.labels_horTop.size(); ++i) {
        self.corner_topRight.push_back(std::string(self.labels_verRightWidth +
                                                       (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                                       Config::axisLabels_padRight_vl,
                                                   Config::space));
    }
    return self;
}
auto BarV::compute_corner_br(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horBottom_bool) {
        self.corner_bottomRight.push_back(
            std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                            Config::axisLabels_padLeft_vr,
                        Config::space));
    }
    for (size_t i = 0; i < self.labels_horBottom.size(); ++i) {
        self.corner_bottomRight.push_back(
            std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                            Config::axisLabels_padRight_vl,
                        Config::space));
    }
    return self;
}
auto BarV::compute_areaCorners(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.areaCorner_tl = Config::areaCorner_tl_barV;
        self.areaCorner_bl = Config::areaCorner_bl_barV;
        self.areaCorner_tr = Config::areaCorner_tr;
        self.areaCorner_br = Config::areaCorner_br;
    }
    else {
        self.areaCorner_tl = Config::areaCorner_tl;
        self.areaCorner_bl = Config::areaCorner_bl;
        self.areaCorner_tr = Config::areaCorner_tr;
        self.areaCorner_br = Config::areaCorner_br;
    }
    return self;
}


auto BarV::compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computePA = [&](auto &var) -> void {
        if constexpr (not std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            // Non-arithmetic types cannot be plotted as values
            assert(false);
        }
        else {
            auto const [minV, maxV] = std::ranges::minmax(var);
            long long scalingFactor;
            if constexpr (std::is_integral_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
                scalingFactor = std::numeric_limits<long long>::max() / (std::max(std::abs(maxV), std::abs(minV)));
            }
            else if constexpr (std::is_floating_point_v<std::ranges::range_value_t<decltype(var)>>) {
                scalingFactor = 1;
            }
            // Is arithmetic but not integral or floating point => impossible
            else { static_assert(false); }

            auto maxV_adj = maxV * scalingFactor;
            auto minV_adj = minV * scalingFactor;
            auto stepSize = (maxV_adj - minV_adj) / (self.areaWidth);

            for (auto const &val : var) {
                long long rpt = (val * scalingFactor - minV_adj) / stepSize;
                self.plotArea.push_back(TermColors::get_basicColor(self.dp.color_basePalette.front()));
                for (long long i = rpt; i > 0; --i) { self.plotArea.back().append("■"); }
                self.plotArea.back().append(Config::color_Axes);

                self.plotArea.back().append(Config::term_setDefault);
                for (long long i = rpt; i < self.areaWidth; ++i) { self.plotArea.back().push_back(Config::space); }
            }
        }
    };

    std::visit(computePA, self.values_data.at(0));
    return self;
}

auto BarV::compute_footer(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if ((not self.dp.display_filtered_bool.has_value()) || self.dp.display_filtered_bool.value() == false) {
        return self;
    }

    auto filteredCount = std::ranges::count_if(self.dp.filterFlags, [](auto const &ff) { return ff != 0; });
    if (filteredCount == 0) { return self; }

    std::string res1{};
    if (std::ranges::any_of(self.dp.filterFlags, [](auto const &ff) { return ff & 0b1; })) {
        res1.push_back('\n');
        res1.append("Warning:\n");
        res1.append("The following rows were filtered out because they contained 'null' values:\n");
        auto viewOfFiltered = std::views::enumerate(self.dp.filterFlags) |
                              std::views::filter([](auto const &pr) { return std::get<1>(pr) & 0b1; }) |
                              std::views::transform([](auto const &&pr2) { return std::get<0>(pr2); });
        for (auto const &f_item : viewOfFiltered) {
            res1.append(std::to_string(f_item));
            res1.push_back(',');
            res1.push_back(' ');
        }
        res1.pop_back();
        res1.pop_back();
        res1.push_back('\n');
    }
    if (std::ranges::any_of(self.dp.filterFlags, [](auto const &ff) { return ff & 0b10; })) {
        res1.append("\n");
        res1.append("Warning:\n");
        res1.append(std::format(
            "The following rows were filtered out because they contained extreme values outside {}σ from mean:\n",
            self.dp.filter_outsideStdDev.value()));
        auto viewOfFiltered = std::views::enumerate(self.dp.filterFlags) |
                              std::views::filter([](auto const &pr) { return std::get<1>(pr) & 0b10; }) |
                              std::views::transform([](auto const &&pr2) { return std::get<0>(pr2); });
        for (auto const &f_item : viewOfFiltered) {
            res1.append(std::to_string(f_item));
            res1.push_back(',');
            res1.push_back(' ');
        }
        res1.pop_back();
        res1.pop_back();
        res1.push_back('\n');
    }
    self.footer = res1;
    return self;
}
// ### END BAR V ###


// BAR VM
auto BarVM::compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // VERTICAL LEFT LABELS SIZE
    auto olset = [](auto &var) -> size_t {
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            return std::ranges::max(std::views::transform(var, [](auto const &a) { return detail::strlen_utf8(a); }));
        }
        else { return Config::max_sizeOfValueLabels; }
    };
    self.labels_verLeftWidth =
        std::min(Config::axisLabels_maxLength_vl,
                 std::min(std::visit(olset, self.labelTS_data.value()),
                          static_cast<size_t>((self.dp.targetWidth.value() - self.pad_left - self.pad_right) / 4)));

    // VERTICAL RIGHT LABELS SIZE
    // Will be used as 'legend' for some types of Plots
    auto selColNameSizes = std::views::transform(
        self.dp.values_colIDs, [&](auto &&colID) { return detail::strlen_utf8(self.ds.m_data.at(colID).name); });

    self.labels_verRightWidth =
        std::min(Config::axisLabels_maxLength_vr,
                 std::min(std::ranges::max(selColNameSizes),
                          static_cast<size_t>((self.dp.targetWidth.value() - self.pad_left - self.pad_right) / 4)));

    // VERTICAL AXES NAMES ... LEFT true if value cols size == 1, RIGHT never

    self.axisName_verLeft_bool  = false;
    self.axisName_verRight_bool = false;

    // PLOT AREA
    // Plot area width (-2 is for the 2 vertical axes positions)
    self.areaWidth = self.dp.targetWidth.value() - self.pad_left -
                     (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2 -
                     self.labels_verRightWidth - (Config::axis_verName_width_vr * self.axisName_verRight_bool) -
                     self.pad_right;
    if (self.areaWidth < static_cast<long long>(Config::min_areaWidth)) {
        return std::unexpected(incerr_c::make(C_DSC_areaWidth_insufficient));
    }

    // LABELS AND AXIS NAME HOR BOTTOM
    if (self.dp.values_colIDs.size() > 1) { self.axisName_horBottom_bool = false; }
    else { self.axisName_horBottom_bool = true; }

    // Labels and axis name top ... probably nothing so keeping 0 size
    // ...

    // PLOT AREA HEIGHT
    self.areaHeight = (self.data_rowCount * self.dp.values_colIDs.size()) + (self.data_rowCount - 1);
    if (self.areaHeight < static_cast<long long>(Config::min_areaHeight)) {
        return std::unexpected(incerr_c::make(C_DSC_areaHeight_insufficient));
    }

    // Axes steps
    self.axis_verLeftSteps   = self.data_rowCount - 1;
    self.axis_horBottomSteps = detail::guess_stepsOnHorAxis(self.areaWidth);

    // Top and Right axes steps keeping as-is

    return self;
}
auto BarVM::compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_verLeft_bool) {
        self.axisName_verLeft =
            detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.labelTS_colID.value()).name, self.areaHeight);
    }
    return self;
}
auto BarVM::compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto olset = [&](auto &var) -> void {
        size_t const cycleSize = self.dp.values_colIDs.size();
        size_t const labelPos  = cycleSize / 2;

        for (auto const &rawLabel : var) {
            // (cycleSize+1) because we need to add additional empty line at the bottom
            for (size_t cycle_i = 0; cycle_i < (cycleSize + 1); ++cycle_i) {
                if (cycle_i == labelPos) {
                    if constexpr (std::same_as<std::string,
                                               std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
                        self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth));
                    }
                    else {
                        self.labels_verLeft.push_back(
                            detail::trim2Size_leading(detail::format_toMax5length(rawLabel), self.labels_verLeftWidth));
                    }
                    for (size_t i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                        self.labels_verLeft.back().push_back(Config::space);
                    }
                }
                else {
                    self.labels_verLeft.push_back(
                        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));
                }
            }
        }
    };

    // Empty label at the top
    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    std::visit(olset, self.labelTS_data.value());

    // No empty label at the bottom because its part of the loop above

    return (self);
}
// labels_vr are actually the legend here
auto BarVM::compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Categories are specified by column names
    if (self.dp.values_colIDs.size() > 1) {
        // horTop axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));

        for (size_t lineID = 0; lineID < static_cast<size_t>(self.areaHeight); ++lineID) {
            if (lineID < (self.dp.values_colIDs.size())) {
                self.labels_verRight.push_back(
                    std::string(Config::axisLabels_padLeft_vr, Config::space)
                        .append(TermColors::get_basicColor(self.dp.color_basePalette.at(lineID)))
                        .append(detail::trim2Size_ending(self.ds.m_data.at(self.dp.values_colIDs.at(lineID)).name,
                                                         self.labels_verRightWidth))
                        .append(Config::term_setDefault));
            }
            else {
                self.labels_verRight.push_back(
                    std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
            }
        }
        // horBottom axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
    }

    // If no categories then all VR labels are empty strings
    else {
        for (int i = 0; i < (self.areaHeight + 2); ++i) { self.labels_verRight.push_back(""); }
    }

    return self;
}

auto BarVM::compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computeLabels = [&](double const &minV, double const &maxV) -> void {
        size_t const fillerSize  = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
        auto         stepSize    = ((maxV - minV) / self.areaWidth);
        size_t       placedChars = 0;

        self.labels_horBottom.push_back(std::string());
        self.labels_horBottom.back().append(Config::color_Axes);

        // Construct the [0:0] point label == minV
        std::string tempStr = detail::format_toMax5length(minV);
        self.labels_horBottom.back().append(tempStr);
        placedChars += detail::strlen_utf8(tempStr);

        // Construct the tick labels
        for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
            tempStr = detail::format_toMax5length(minV + ((i + 1) * (fillerSize + 1) * stepSize));

            while (placedChars < (i * (fillerSize + 1) + fillerSize + (tempStr.size() < 3) - (tempStr.size() > 4))) {
                self.labels_horBottom.back().push_back(Config::space);
                placedChars++;
            }

            self.labels_horBottom.back().append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);
        }

        // Construct the [0:end] point label
        tempStr = detail::format_toMax5length(maxV);
        for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
            self.labels_horBottom.back().push_back(Config::space);
        }
        self.labels_horBottom.back().append(tempStr);
        self.labels_horBottom.back().append(Config::term_setDefault);
    };

    auto [minV, maxV] = incom::standard::algos::compute_minMaxMulti(self.values_data);
    computeLabels(minV, maxV);

    return self;
}
auto BarVM::compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto const [minV, maxV]         = incom::standard::algos::compute_minMaxMulti(self.values_data);
    auto const        bigStepSize   = (maxV - minV) / (self.areaWidth);
    auto const        smallStepSize = bigStepSize / 8;
    size_t const      skipSize      = self.values_data.size() + 1;
    std::string const emptyPlotLine(self.areaWidth, Config::space);

    // Series count * row count + 'empty lines' (row count - 1)
    self.plotArea =
        std::vector((self.values_data.size() * self.data_rowCount) + self.data_rowCount - 1, std::string{""});

    auto computePA = [&, seriesID = 0uz](auto &var) mutable -> void {
        if constexpr (not std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            // Non-arithmetic types cannot be plotted as values
            assert(false);
        }
        else {
            std::u32string tmp_u32string;
            for (size_t rowID = 0; auto const &val : var) {
                long long const bigSteps   = (val - minV) / bigStepSize;
                long long const smallSteps = ((val - minV) - (bigSteps * bigStepSize)) / smallStepSize;

                size_t barsPlaced = 1;
                for (long long i = bigSteps; i > 0; --i) {
                    tmp_u32string.push_back(Config::blocks_hor[8]);
                    barsPlaced++;
                }
                tmp_u32string.push_back(Config::blocks_hor[smallSteps]);

                for (; barsPlaced < self.areaWidth; ++barsPlaced) { tmp_u32string.push_back(Config::blocks_hor[0]); }

                if (rowID != 0 && seriesID == 0) {
                    self.plotArea.at((rowID * skipSize) - 1).append(Config::term_setDefault);
                    self.plotArea.at((rowID * skipSize) - 1).append(emptyPlotLine);
                }

                std::string &lineRef = self.plotArea.at((rowID * skipSize) + seriesID);
                lineRef.append(TermColors::get_basicColor(self.dp.color_basePalette.at(seriesID)));
                lineRef.append(incom::terminal_plot::detail::convert_u32u8(tmp_u32string));
                lineRef.append(Config::term_setDefault);

                tmp_u32string.clear();
                rowID++;
            }
        }
        seriesID++;
    };

    for (auto const &varVec : self.values_data) { std::visit(computePA, varVec); }


    return self;
}
// ### END BAR VM ###


// SCATTER
auto Scatter::compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_verLeft_bool) {
        if (self.dp.values_colIDs.size() == 1) {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.values_colIDs.at(0)).name, self.areaHeight);
        }
    }
    return self;
}

auto Scatter::compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto getValLabels = [&](size_t areaLength, size_t const &labelsWidth, size_t const padRight, size_t const padLeft) {
        auto const  fillerLength = detail::get_axisFillerSize(areaLength, self.axis_verLeftSteps);
        std::string filler(labelsWidth, Config::space);

        // Min and Max are not actually at the position of horizontal axes, but one smallStep below and above
        auto [minVal, maxVal] = incom::standard::algos::compute_minMaxMulti(self.values_data);
        auto stepSize         = (maxVal - minVal) / (areaLength - 0.25);
        minVal                = minVal - (stepSize / 4);
        maxVal                = maxVal + (stepSize / 4);

        // Construct with 'left padding' in place
        std::vector<std::string> res(areaLength + 2, std::string(padLeft, Config::space).append(Config::color_Axes));

        // Value label of 'zero point'
        res.front().append(detail::trim2Size_leading(detail::format_toMax5length(minVal), labelsWidth));

        for (size_t id = 0; id < self.axis_verLeftSteps; ++id) {
            for (size_t fillID = 0; fillID < fillerLength; ++fillID) {
                res.at(id * (fillerLength + 1) + fillID + 1).append(filler);
            }
            // Value label at the current position
            res.at((id + 1) * (fillerLength + 1))
                .append(detail::trim2Size_leading(
                    detail::format_toMax5length(minVal + (stepSize * (id + 1) * (fillerLength + 1))), labelsWidth));
        }

        // Filler up to 'max point'
        for (size_t i = self.axis_verLeftSteps * (fillerLength + 1) + 1; i < res.size() - 1; ++i) {
            res.at(i).append(filler);
        }

        // Value label of 'max point'
        res.back().append(detail::trim2Size_leading(detail::format_toMax5length(maxVal), labelsWidth));
        for (auto &line : res) {
            for (size_t i = 0; i < padRight; ++i) { line.push_back(Config::space); }
        }
        std::ranges::reverse(res);
        return res;
    };


    self.labels_verLeft = getValLabels(self.areaHeight, self.labels_verLeftWidth, Config::axisLabels_padRight_vl, 0);

    return (self);
}
// labels_vr are actually the legend here
auto Scatter::compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Categories are specified by catColID
    if (self.dp.cat_colID.has_value()) {
        auto create_catIDs_vec = [&](auto &view) -> std::vector<std::string> {
            auto sortedUniqued = detail::get_sortedAndUniqued(view);
            if constexpr (std::same_as<std::string,
                                       std::ranges::range_value_t<std::remove_cvref_t<decltype(sortedUniqued)>>>) {
                return sortedUniqued;
            }
            else {
                std::vector<std::string> res;
                for (auto const &suItem : sortedUniqued) { res.push_back(std::to_string(suItem)); }
                return res;
            }
        };
        auto uniquedCats_vec = std::visit(create_catIDs_vec, self.cat_data.value());
        // horTop axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));

        for (size_t lineID = 0; lineID < static_cast<size_t>(self.areaHeight); ++lineID) {
            if (lineID < uniquedCats_vec.size()) {
                self.labels_verRight.push_back(
                    std::string(Config::axisLabels_padLeft_vr, Config::space)
                        .append(TermColors::get_basicColor(self.dp.color_basePalette.at(lineID)))
                        .append(detail::trim2Size_ending(uniquedCats_vec.at(lineID), self.labels_verRightWidth))
                        .append(Config::term_setDefault));
            }
            else {
                self.labels_verRight.push_back(
                    std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
            }
        }
        // horBottom axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
    }

    // Categories are specified by column names
    else if (self.dp.values_colIDs.size() > 1) {
        // horTop axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));

        for (size_t lineID = 0; lineID < static_cast<size_t>(self.areaHeight); ++lineID) {
            if (lineID < (self.dp.values_colIDs.size())) {
                self.labels_verRight.push_back(
                    std::string(Config::axisLabels_padLeft_vr, Config::space)
                        .append(TermColors::get_basicColor(self.dp.color_basePalette.at(lineID)))
                        .append(detail::trim2Size_ending(self.ds.m_data.at(self.dp.values_colIDs.at(lineID)).name,
                                                         self.labels_verRightWidth))
                        .append(Config::term_setDefault));
            }
            else {
                self.labels_verRight.push_back(
                    std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
            }
        }
        // horBottom axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));
    }

    // If no categories then all VR labels are empty strings
    else {
        for (int i = 0; i < (self.areaHeight + 2); ++i) { self.labels_verRight.push_back(""); }
    }

    return self;
}

auto Scatter::compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = detail::create_tickMarkedAxis(Config::axisFiller_r, Config::axisTick_r, 0, self.areaHeight);
    return self;
}

auto Scatter::compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    for (long long i = 0; i < self.areaWidth; ++i) { self.axis_horTop.push_back(Config::axisFiller_t); }
    return self;
}

auto Scatter::compute_axisName_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Name of the TS column
    self.axisName_horBottom =
        detail::trim2Size_leadingEnding(self.ds.m_data.at(self.dp.labelTS_colID.value()).name, self.areaWidth);
    return self;
}
auto Scatter::compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computeLabels = [&](double const &minV, double const &maxV) -> void {
        size_t const fillerSize  = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
        auto         stepSize    = ((maxV - minV) / ((2 * self.areaWidth) - 1)) * 2;
        size_t       placedChars = 0;

        self.labels_horBottom.push_back(std::string());
        self.labels_horBottom.back().append(Config::color_Axes);

        // Construct the [0:0] point label (minV - stepsize to make the first label one below the minV)
        std::string tempStr = detail::format_toMax5length(minV - stepSize);
        self.labels_horBottom.back().append(tempStr);
        placedChars += detail::strlen_utf8(tempStr);

        // Construct the tick labels
        for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
            tempStr = detail::format_toMax5length((minV - stepSize) + ((i + 1) * (fillerSize + 1) * stepSize));

            while (placedChars < (i * (fillerSize + 1) + fillerSize + (tempStr.size() < 3) - (tempStr.size() > 4))) {
                self.labels_horBottom.back().push_back(Config::space);
                placedChars++;
            }

            self.labels_horBottom.back().append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);
        }

        // Construct the [0:end] point label
        tempStr = detail::format_toMax5length(maxV + (stepSize / 2));
        for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
            self.labels_horBottom.back().push_back(Config::space);
        }
        self.labels_horBottom.back().append(tempStr);
        self.labels_horBottom.back().append(Config::term_setDefault);
    };

    auto [minV, maxV] = incom::standard::algos::compute_minMaxMulti(self.labelTS_data.value());
    computeLabels(minV, maxV);

    return self;
}
auto Scatter::compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    std::optional<std::vector<size_t>> opt_catIDs_vec = std::nullopt;
    if (self.dp.cat_colID.has_value()) {
        auto create_catIDs_vec = [&](auto &vec) -> std::vector<size_t> {
            auto                catVals_uniqued = detail::get_sortedAndUniqued(vec);
            std::vector<size_t> catIDs_vec;

            for (auto const &catVal : vec) {
                // Push_back catValue's designated ID into a dedicated vector
                catIDs_vec.push_back(std::ranges::find_if(catVals_uniqued, [&](auto const &a) { return a == catVal; }) -
                                     catVals_uniqued.begin());
            }
            return catIDs_vec;
        };

        opt_catIDs_vec = std::visit(create_catIDs_vec, self.cat_data.value());
    }

    self.plotArea = detail::BrailleDrawer::drawPoints(self.areaWidth, self.areaHeight, self.labelTS_data,
                                                      self.values_data, opt_catIDs_vec, self.dp.color_basePalette);

    return self;
}

// ### END SCATTER ###


// MULTILINE

auto Multiline::compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = std::vector(self.areaHeight, std::string(" "));
    return self;
}

auto Multiline::compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Below is a trick to call a member function as if 'self' was its parent type
    // return (&BarV::template compute_axis_ht<std::remove_cvref_t<decltype(self)>>)(std::move(self));

    self.axis_horTop = std::vector(self.areaWidth, std::string(" "));
    return self;
}

auto Multiline::compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    self.plotArea = detail::BrailleDrawer::drawLines(self.areaWidth, self.areaHeight, self.labelTS_data.value(),
                                                     self.values_data, self.dp.color_basePalette);
    return self;
}
// ### END MULTILINE ###


// BAR HM
auto BarHM::compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // VERTICAL LEFT LABELS SIZE
    self.labels_verLeftWidth = Config::max_valLabelSize;

    // VERTICAL RIGHT LABELS SIZE
    // Will be used as 'legend' for some types of Plots
    auto selColNameSizes = std::views::transform(
        self.dp.values_colIDs, [&](auto &&colID) { return detail::strlen_utf8(self.ds.m_data.at(colID).name); });

    self.labels_verRightWidth =
        std::min(Config::axisLabels_maxLength_vr,
                 std::min(std::ranges::max(selColNameSizes),
                          static_cast<size_t>((self.dp.targetWidth.value() - self.pad_left - self.pad_right) / 4)));

    // VERTICAL AXES NAMES ... LEFT if values col size == 1, RIGHT never
    if (self.dp.values_colIDs.size() > 1) { self.axisName_verLeft_bool = false; }
    else { self.axisName_verLeft_bool = true; }
    self.axisName_verRight_bool = false;

    // PLOT AREA
    // Plot area width (-2 is for the 2 vertical axes positions)
    self.areaWidth = self.dp.targetWidth.value() - self.pad_left -
                     (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2ll -
                     self.labels_verRightWidth - (Config::axis_verName_width_vr * self.axisName_verRight_bool) -
                     self.pad_right;
    if (self.areaWidth < static_cast<long long>(Config::min_areaWidth_BarHM) ||
        self.areaWidth < ((self.data_rowCount * self.values_data.size()) + self.data_rowCount - 1)) {
        return std::unexpected(incerr_c::make(C_DSC_areaWidth_insufficient));
    }
    else { self.areaWidth = ((self.data_rowCount * self.values_data.size()) + self.data_rowCount - 1); }

    // LABELS AND AXIS NAME HOR BOTTOM
    self.axisName_horBottom_bool = false;

    // LABELS AND AXIS NAME HOR TOP ... probably nothing so keeping 0 size
    // ...

    // PLOT AREA HEIGHT
    if (not self.dp.targetHeight.has_value()) {
        if (not self.dp.availableWidth.has_value() || not self.dp.availableHeight.has_value()) {
            self.areaHeight = self.areaWidth / Config::default_areaWidth2Height_ratio_BarHM;
        }
        else {
            self.areaHeight = self.areaWidth *
                              (static_cast<double>(self.dp.availableHeight.value()) / self.dp.availableWidth.value());
        }
    }
    else {
        self.areaHeight = static_cast<long long>(self.dp.targetHeight.value()) - self.pad_top -
                          self.axisName_horTop_bool - self.labels_horTop.size() - 2ll - self.labels_horBottom.size() -
                          self.axisName_horBottom_bool - self.pad_bottom;
    }
    if (self.areaHeight < static_cast<long long>(Config::min_areaHeight)) {
        return std::unexpected(incerr_c::make(C_DSC_areaHeight_insufficient));
    }

    // Axes steps

    self.axis_verLeftSteps   = detail::guess_stepsOnVerAxis(self.areaHeight);
    self.axis_horBottomSteps = self.data_rowCount - 1;

    // Top and Right axes steps keeping as-is

    return self;
}

auto BarHM::compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto olset = [](auto &var) -> size_t {
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            return std::ranges::max(std::views::transform(var, [](auto const &a) { return detail::strlen_utf8(a); }));
        }
        else if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            return Config::max_sizeOfValueLabels;
        }
        else { static_assert(false); } // This should never be instantiated
    };
    auto const realMaxLabelSize = std::visit(olset, self.labelTS_data.value());

    // If max label size is small enough we will make all 1 width vertical (usually used for arithmetic values)
    bool const   pureVertical = realMaxLabelSize <= Config::max_sizeOfValueLabels;
    size_t const labelWidth   = pureVertical ? 1 : self.values_data.size();

    // '(realMaxLabelSize + labelWidth - 1) / labelWidth)' means rounded up to nearest integer
    size_t const labelHeight =
        std::min(Config::axisLabels_maxHeight_hb, (realMaxLabelSize + labelWidth - 1) / labelWidth);

    size_t const labelCharCount = labelHeight * (pureVertical ? 1 : labelWidth);

    auto computeLabels = [&](auto &var) -> void {
        size_t const label_startHorPos = pureVertical ? self.values_data.size() / 2 : 0;
        size_t const label_horSize     = pureVertical ? 1 : self.values_data.size();

        std::ranges::fill_n(std::back_inserter(self.labels_horBottom), labelHeight, std::string{Config::color_Axes});
        for (auto &lab_line : self.labels_horBottom) { lab_line.push_back(Config::space); }

        std::vector<std::string> tmpHolder;
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            tmpHolder = std::vector<std::string>(std::from_range, std::views::transform(var, [&](auto &&oneValue) {
                                                     return detail::trim2Size_ending(oneValue, labelCharCount);
                                                 }));
        }
        else if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            tmpHolder = std::vector<std::string>(std::from_range, std::views::transform(var, [&](auto &&oneValue) {
                                                     return detail::trim2Size_ending(
                                                         detail::format_toMax5length(oneValue), labelCharCount);
                                                 }));
        }
        else { static_assert(false); } // This should never be instantiated

        for (size_t startOffset = 0; auto &res_oneLine : self.labels_horBottom) {
            for (auto const &tmpLine : tmpHolder) {
                res_oneLine.append(label_startHorPos, Config::space);
                res_oneLine.append(tmpLine.begin() + startOffset, tmpLine.begin() + startOffset + label_horSize);
                res_oneLine.append(self.values_data.size() - labelWidth - label_startHorPos+1, Config::space);
            }
            startOffset += label_horSize;
        }
    };
    std::visit(computeLabels, self.labelTS_data.value());

    // Add one space at the end of each label line (aligned with vertical right axis)
    // Set terminal colour back to default on each line
    for (auto &labelLine : self.labels_horBottom) {
        labelLine.push_back(Config::space);
        labelLine.append(Config::term_setDefault);
    }


    return self;
}

auto BarHM::compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto [minV, maxV]        = incom::standard::algos::compute_minMaxMulti(self.values_data);
    auto       bigStepSize   = (maxV - minV) / (self.areaHeight - 0.125);
    auto const smallStepSize = bigStepSize / 8;
    minV                     = minV - (bigStepSize / 8);
    size_t const skipSize    = self.values_data.size() + 1;

    self.plotArea = std::vector(static_cast<size_t>(self.areaHeight), std::string{});

    // levels: 0 = valuesCols, 1 = rowIDs_inPlot, 2 = lineOfVals
    std::vector symbolVects(self.values_data.size(), std::vector(self.areaHeight, std::vector<size_t>{}));

    for (size_t valColID = 0; auto const &valCol : self.values_data) {
        auto symbolVectorCreator = [&](auto const &valCol) {
            if constexpr (std::is_arithmetic_v<std::ranges::range_value_t<std::remove_cvref_t<decltype(valCol)>>>) {
                auto &symbolVecOfRows = symbolVects.at(valColID);
                for (size_t rowID_ID = 0; auto &symbol_oneRow : std::views::reverse(symbolVecOfRows)) {
                    auto const bottomLvl = minV + (rowID_ID * bigStepSize);
                    for (auto const &oneValue : valCol) {
                        if (oneValue < bottomLvl) { symbol_oneRow.push_back(0uz); }
                        else if (oneValue >= (bottomLvl + bigStepSize)) { symbol_oneRow.push_back(8uz); }
                        else { symbol_oneRow.push_back((oneValue - bottomLvl) / smallStepSize); }
                    }
                    rowID_ID++;
                }
            }
            else { assert(false); }
        };

        std::visit(symbolVectorCreator, valCol);
        valColID++;
    }


    for (size_t resLine_ID = 0; auto &resLine : self.plotArea) {
        std::string &resLineRef = self.plotArea.at(resLine_ID);

        for (size_t col_inPlotID = 0; col_inPlotID < self.data_rowCount; ++col_inPlotID) {
            for (size_t valColID = 0; valColID < self.values_data.size(); ++valColID) {

                resLineRef.append(TermColors::get_basicColor(self.dp.color_basePalette.at(valColID)));
                resLineRef.append(Config::blocks_ver_str.at(symbolVects[valColID][resLine_ID][col_inPlotID]));
            }
            resLineRef.push_back(Config::space);
        }
        resLineRef.pop_back();
        resLine_ID++;
    }


    return self;
}
// ### END BAR HM ###

} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom