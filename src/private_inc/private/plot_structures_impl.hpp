#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <expected>
#include <functional>
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
auto Base::build_self(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Can only build it from rvalue ...
    if constexpr (std::is_lvalue_reference_v<decltype(self)>) { static_assert(false); }

    using self_t = std::remove_cvref_t<decltype(self)>;

    return std::move(self)
        .initialize_data_views(dp, ds)

        .and_then(std::bind_back(&self_t::template compute_descriptors<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template validate_descriptors<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_axisName_vl<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_axisName_vr<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_labels_vl<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_labels_vr<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_axis_vl<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_axis_vr<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_corner_tl<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_corner_bl<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_corner_br<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_corner_tr<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_areaCorners<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_axis_ht<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_axisName_ht<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_labels_ht<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_axis_hb<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_axisName_hb<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_labels_hb<self_t>, dp, ds))

        .and_then(std::bind_back(&self_t::template compute_plot_area<self_t>, dp, ds))
        .and_then(std::bind_back(&self_t::template compute_footer<self_t>, dp, ds));
}
inline size_t Base::compute_lengthOfSelf() const {

    size_t lngth = pad_top + pad_bottom;

    // Horizontal top axis name and axis labels lines
    lngth += axisName_horTop_bool ? (axisName_horTop.size() + corner_topLeft.front().size() +
                                     corner_topRight.front().size() + pad_left + pad_right)
                                  : 0;
    lngth += labels_horTop_bool ? (label_horTop.size() + corner_topLeft.front().size() +
                                   corner_topRight.front().size() + pad_left + pad_right)
                                : 0;

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
    lngth += labels_horBottom_bool ? (label_horBottom.size() + corner_bottomLeft.front().size() +
                                      corner_bottomRight.front().size() + pad_left + pad_right)
                                   : 0;
    lngth += axisName_horBottom_bool ? (axisName_horBottom.size() + corner_bottomLeft.front().size() +
                                        corner_bottomRight.front().size() + pad_left + pad_right)
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
        result.append(label_horTop);
        result.append(corner_topRight.front());
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }
    if (labels_horTop_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_topLeft.back());
        result.append(label_horTop);
        result.append(corner_topRight.back());
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
    if (labels_horBottom_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_bottomLeft.front());
        result.append(label_horBottom);
        result.append(corner_bottomRight.front());
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

auto BarV::initialize_data_views(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    if (dp.labelTS_colID.has_value()) {
        auto dataView = ds.get_filteredViewOfData(dp.labelTS_colID.value(), dp.filterFlags);

        auto create_LOC_storage = [&](auto &var) { self.labelTS_data = std::ranges::to<std::vector>(var); };
        std::visit(create_LOC_storage, dataView);
    }
    else { return std::unexpected(incerr_c::make(INI_labelTS_colID_isNull)); }

    if (dp.cat_colID.has_value()) {
        auto dataView = ds.get_filteredViewOfData(dp.cat_colID.value(), dp.filterFlags);

        auto create_LOC_storage = [&](auto &var) { self.cat_data = std::ranges::to<std::vector>(var); };
        std::visit(create_LOC_storage, dataView);
    }

    if (dp.values_colIDs.size() == 0) { return std::unexpected(incerr_c::make(INI_values_colIDs_isEmpty)); }
    else {
        auto dataViews = ds.get_filteredViewOfData(dp.values_colIDs, dp.filterFlags);
        for (auto &viewRef : dataViews) {
            auto create_LOC_storage = [&](auto &var) { self.values_data.push_back(std::ranges::to<std::vector>(var)); };
            std::visit(create_LOC_storage, viewRef);
        }
    }

    return self;
}

auto BarV::compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // VERTICAL LEFT LABELS SIZE
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        // auto &labelColVarRef = ds.m_data.at(dp.labelTS_colID.value()).variant_data;

        auto olset = [](auto &var) -> size_t {
            if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
                return std::ranges::max(
                    std::views::transform(var, [](auto const &a) { return detail::strlen_utf8(a); }));
            }
            else { return Config::max_sizeOfValueLabels; }
        };

        auto const maxLabelSize = std::visit(olset, self.labelTS_data.value());

        self.labels_verLeftWidth = std::min(
            Config::axisLabels_maxLength_vl,
            std::min(maxLabelSize, static_cast<size_t>((dp.targetWidth.value() - self.pad_left - self.pad_right) / 4)));
    }
    else { self.labels_verLeftWidth = Config::max_valLabelSize; }

    // VERTICAL RIGHT LABELS SIZE
    // Will be used as 'legend' for some types of Plots
    if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
        // catCol is specified meaning the legend will be uniqued values from that column
        if (dp.cat_colID.has_value()) {
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
        else if (dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : dp.values_colIDs) { maxSize = std::max(maxSize, ds.m_data.at(colID).name.size()); }
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
        else { self.labels_verRightWidth = 0; }
    }
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
        if (dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : dp.values_colIDs) { maxSize = std::max(maxSize, ds.m_data.at(colID).name.size()); }
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
    }
    else { self.labels_verRightWidth = 0; }

    // VERTICAL AXES NAMES ... LEFT always, RIGHT never
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.axisName_verLeft_bool  = false;
        self.axisName_verRight_bool = false;
    }
    else if (dp.values_colIDs.size() > 1) {
        self.axisName_verLeft_bool  = false;
        self.axisName_verRight_bool = false;
    }
    else {
        self.axisName_verLeft_bool  = true;
        self.axisName_verRight_bool = false;
    }

    // PLOT AREA
    // Plot area width (-2 is for the 2 vertical axes positions)
    self.areaWidth = dp.targetWidth.value() - self.pad_left -
                     (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2 -
                     self.labels_verRightWidth - (Config::axis_verName_width_vr * self.axisName_verRight_bool) -
                     self.pad_right;
    if (self.areaWidth < static_cast<long long>(Config::min_areaWidth)) {
        return std::unexpected(incerr_c::make(C_DSC_areaWidth_insufficient));
    }

    // Labels and axis name bottom
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {} // TODO: Proper assessment for Multiline
    else {
        self.labels_horBottom_bool   = true;
        self.axisName_horBottom_bool = true;
    }

    // Labels and axis name top ... probably nothing so keeping 0 size
    // ...

    // Plot area height (-2 is for the 2 horizontal axes positions)
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {


        self.areaHeight = std::visit([](auto &a) { return std::ranges::count_if(a, [](auto &&a2) { return true; }); },
                                     self.values_data.at(0));
    }
    else if (not dp.targetHeight.has_value()) {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            //
            if (not dp.availableWidth.has_value() || not dp.availableHeight.has_value()) {
                self.areaHeight = self.areaWidth / 6;
            }
            else {
                self.areaHeight =
                    self.areaWidth * (static_cast<double>(dp.availableHeight.value()) / dp.availableWidth.value());
            }
        }
        else {
            if (not dp.availableWidth.has_value() || not dp.availableHeight.has_value()) {
                self.areaHeight = self.areaWidth / 3;
            }
            else {
                self.areaHeight =
                    self.areaWidth * (static_cast<double>(dp.availableHeight.value()) / dp.availableWidth.value());
            }
        }
    }
    else {
        self.areaHeight = static_cast<long long>(dp.targetHeight.value()) - self.pad_top - self.axisName_horTop_bool -
                          self.labels_horTop_bool - 2ll - self.labels_horBottom_bool - self.axisName_horBottom_bool -
                          self.pad_bottom;
    }
    if (self.areaHeight < static_cast<long long>(Config::min_areaHeight)) {
        return std::unexpected(incerr_c::make(C_DSC_areaHeight_insufficient));
    }

    // Axes steps
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        self.axis_verLeftSteps = self.areaHeight;
    }
    else { self.axis_verLeftSteps = detail::guess_stepsOnVerAxis(self.areaHeight); }


    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
        self.axis_horBottomSteps = self.areaWidth;
    }
    else { self.axis_horBottomSteps = detail::guess_stepsOnHorAxis(self.areaWidth); }

    // Top and Right axes steps keeping as-is

    return self;
}

auto BarV::compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_verLeft_bool) {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(ds.m_data.at(dp.labelTS_colID.value()).name, self.areaHeight);
        }
        else {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(ds.m_data.at(dp.values_colIDs.at(0)).name, self.areaHeight);
        }
    }
    return self;
}
auto BarV::compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}

auto BarV::compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Empty label at the top
    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    auto olset = [&](auto &var) -> void {
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            for (auto const &rawLabel : var) {
                self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth));
                for (size_t i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                    self.labels_verLeft.back().push_back(Config::space);
                }
            }
        }
        else {
            for (auto const &rawLabelNumeric : var) {
                self.labels_verLeft.push_back(
                    detail::trim2Size_leading(detail::format_toMax5length(rawLabelNumeric), self.labels_verLeftWidth));
                for (size_t i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                    self.labels_verLeft.back().push_back(Config::space);
                }
            }
        }
    };

    std::visit(olset, self.labelTS_data.value());

    // Empty label at the bottom
    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    return (self);
}
auto BarV::compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    for (int i = 0; i < (self.areaHeight + 2); ++i) { self.labels_verRight.push_back(""); }
    return self;
}

auto BarV::compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
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
auto BarV::compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = std::vector(self.areaHeight, std::string(" "));
    return self;
}
// All corners are simply empty as default ... but can possibly be used for something later if overriden in
// derived
auto BarV::compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horTop_bool) {
        self.corner_topLeft.push_back(std::string(self.labels_verLeftWidth +
                                                      (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                      Config::axisLabels_padRight_vl,
                                                  Config::space));
    }
    if (self.labels_horTop_bool) {
        self.corner_topLeft.push_back(std::string(self.labels_verLeftWidth +
                                                      (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                      Config::axisLabels_padRight_vl,
                                                  Config::space));
    }

    return self;
}
auto BarV::compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horBottom_bool) {
        self.corner_bottomLeft.push_back(std::string(self.labels_verLeftWidth +
                                                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                         Config::axisLabels_padRight_vl,
                                                     Config::space));
    }
    if (self.labels_horBottom_bool) {
        self.corner_bottomLeft.push_back(std::string(self.labels_verLeftWidth +
                                                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                                         Config::axisLabels_padRight_vl,
                                                     Config::space));
    }

    return self;
}
auto BarV::compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horTop_bool) {
        self.corner_topRight.push_back(std::string(self.labels_verRightWidth +
                                                       (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                                       Config::axisLabels_padLeft_vr,
                                                   Config::space));
    }
    if (self.labels_horTop_bool) {
        self.corner_topRight.push_back(std::string(self.labels_verRightWidth +
                                                       (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                                       Config::axisLabels_padLeft_vr,
                                                   Config::space));
    }

    return self;
}
auto BarV::compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_horBottom_bool) {
        self.corner_bottomRight.push_back(
            std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                            Config::axisLabels_padLeft_vr,
                        Config::space));
    }
    if (self.labels_horBottom_bool) {
        self.corner_bottomRight.push_back(
            std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                            Config::axisLabels_padLeft_vr,
                        Config::space));
    }

    return self;
}
auto BarV::compute_areaCorners(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
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


auto BarV::compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_horTop = std::vector(self.areaWidth, std::string(" "));
    return self;
}
auto BarV::compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}
auto BarV::compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    return self;
}


auto BarV::compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
        self.axis_horBottom =
            detail::create_tickMarkedAxis(Config::axisFiller_b, Config::axisTick_b, self.areaWidth, self.areaWidth);
    }
    else {
        // Axis with ticks is contructed according to num of 'steps' which is the num of ticks and the areaWidth
        self.axis_horBottom = detail::create_tickMarkedAxis(Config::axisFiller_b, Config::axisTick_b,
                                                            self.axis_horBottomSteps, self.areaWidth);
    }
    return self;
}
auto BarV::compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Name of the FIRST value column
    self.axisName_horBottom =
        detail::trim2Size_leadingEnding(ds.m_data.at(dp.values_colIDs.at(0)).name, self.areaWidth);
    return self;
}
auto BarV::compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computeLabels = [&](auto &var) -> void {
        if constexpr (std::same_as<std::string, std::ranges::range_value_t<std::remove_cvref_t<decltype(var)>>>) {
            assert(false);
        }
        else {
            size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
            auto const [minV, maxV] = std::ranges::minmax(var);
            auto   stepSize         = (maxV - minV) / static_cast<double>((self.areaWidth));
            size_t placedChars      = 0;

            self.label_horBottom.append(Config::color_Axes);

            // Construct the [0:0] point label
            std::string tempStr = detail::format_toMax5length(minV);
            self.label_horBottom.append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);

            // Construct the tick labels
            for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
                while (placedChars < (i * (fillerSize + 1) + fillerSize)) {
                    self.label_horBottom.push_back(Config::space);
                    placedChars++;
                }
                tempStr = detail::format_toMax5length(minV + ((i * (fillerSize + 1) + fillerSize) * stepSize));
                self.label_horBottom.append(tempStr);
                placedChars += detail::strlen_utf8(tempStr);
            }

            // Construct the [0:end] point label
            tempStr = detail::format_toMax5length(maxV + stepSize);
            for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
                self.label_horBottom.push_back(Config::space);
            }
            self.label_horBottom.append(tempStr);
            self.label_horBottom.append(Config::term_setDefault);
        }
    };
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
        self.label_horBottom = std::string(self.areaWidth + 2, Config::space);
    }
    else { std::visit(computeLabels, self.values_data.at(0)); }

    return self;
}


auto BarV::compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

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
                self.plotArea.push_back(TermColors::get_basicColor(dp.color_basePalette.front()));
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

auto BarV::compute_footer(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if ((not dp.display_filtered_bool.has_value()) || dp.display_filtered_bool.value() == false) { return self; }

    auto filteredCount = std::ranges::count_if(dp.filterFlags, [](auto const &ff) { return ff != 0; });
    if (filteredCount == 0) { return self; }

    std::string res1{};
    if (std::ranges::any_of(dp.filterFlags, [](auto const &ff) { return ff & 0b1; })) {
        res1.push_back('\n');
        res1.append("Warning:\n");
        res1.append("The following rows were filtered out because they contained 'null' values:\n");
        auto viewOfFiltered = std::views::enumerate(dp.filterFlags) |
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
    if (std::ranges::any_of(dp.filterFlags, [](auto const &ff) { return ff & 0b10; })) {
        res1.append("\n");
        res1.append("Warning:\n");
        res1.append(std::format(
            "The following rows were filtered out because they contained extreme values outside {}σ from mean:\n",
            dp.filter_outsideStdDev.value()));
        auto viewOfFiltered = std::views::enumerate(dp.filterFlags) |
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

// BAR H
// ### END BAR H ###

// SCATTER
auto Scatter::compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    if (self.axisName_verLeft_bool) {
        if (dp.values_colIDs.size() == 1) {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(ds.m_data.at(dp.values_colIDs.at(0)).name, self.areaHeight);
        }
    }
    return self;
}

auto Scatter::compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto getValLabels = [&](double const &minVal, double const &maxVal, size_t areaLength, size_t const &labelsWidth,
                            size_t const padRight, size_t const padLeft) {
        auto const  fillerLength = detail::get_axisFillerSize(areaLength, self.axis_verLeftSteps);
        std::string filler(labelsWidth, Config::space);
        auto        stepSize = (maxVal - minVal) / (areaLength + 1);

        // Construct with 'left padding' in place
        std::vector<std::string> res(areaLength + 2, std::string(padLeft, Config::space).append(Config::color_Axes));

        // Value label of 'zero point'
        res.front().append(detail::trim2Size_leading(detail::format_toMax5length(minVal), labelsWidth));

        for (size_t id = 0; id < self.axis_verLeftSteps; ++id) {
            for (size_t fillID = 0; fillID < fillerLength; ++fillID) {
                res.at(id * (fillerLength + 1) + fillID + 1).append(filler);
            }
            // Value label at the current position
            res.at(id * (fillerLength + 1) + fillerLength + 1)
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

    auto [minV, maxV] = detail::compute_minMaxMulti(self.values_data);
    self.labels_verLeft =
        getValLabels(minV, maxV, self.areaHeight, self.labels_verLeftWidth, Config::axisLabels_padRight_vl, 0);

    return (self);
}
// labels_vr are actually the legend here
auto Scatter::compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Categories are specified by catColID
    if (dp.cat_colID.has_value()) {
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
                        .append(TermColors::get_basicColor(dp.color_basePalette.at(lineID)))
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
    else if (dp.values_colIDs.size() > 1) {
        // horTop axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));

        for (size_t lineID = 0; lineID < static_cast<size_t>(self.areaHeight); ++lineID) {
            if (lineID < (dp.values_colIDs.size())) {
                self.labels_verRight.push_back(
                    std::string(Config::axisLabels_padLeft_vr, Config::space)
                        .append(TermColors::get_basicColor(dp.color_basePalette.at(lineID)))
                        .append(detail::trim2Size_ending(ds.m_data.at(dp.values_colIDs.at(lineID)).name,
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

auto Scatter::compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = detail::create_tickMarkedAxis(Config::axisFiller_r, Config::axisTick_r, 0, self.areaHeight);
    return self;
}

auto Scatter::compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    for (long long i = 0; i < self.areaWidth; ++i) { self.axis_horTop.push_back(Config::axisFiller_t); }
    return self;
}

auto Scatter::compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Name of the TS column
    self.axisName_horBottom =
        detail::trim2Size_leadingEnding(ds.m_data.at(dp.labelTS_colID.value()).name, self.areaWidth);
    return self;
}
auto Scatter::compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {


    auto computeLabels = [&](double const &minV, double const &maxV) -> void {
        size_t const fillerSize  = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
        auto         stepSize    = ((maxV - minV) / ((2 * self.areaWidth) + 1)) * 2;
        size_t       placedChars = 0;

        self.label_horBottom.append(Config::color_Axes);

        // Construct the [0:0] point label (minV - stepsize to make the first label one below the minV)
        std::string tempStr = detail::format_toMax5length(minV - stepSize);
        self.label_horBottom.append(tempStr);
        placedChars += detail::strlen_utf8(tempStr);

        // Construct the tick labels
        for (size_t i = 0; i < self.axis_horBottomSteps; ++i) {
            tempStr = detail::format_toMax5length((minV - stepSize) + ((i * (fillerSize + 1) + fillerSize) * stepSize));

            while (placedChars < (i * (fillerSize + 1) + fillerSize + (tempStr.size() < 3) - (tempStr.size() > 4))) {
                self.label_horBottom.push_back(Config::space);
                placedChars++;
            }

            self.label_horBottom.append(tempStr);
            placedChars += detail::strlen_utf8(tempStr);
        }

        // Construct the [0:end] point label
        tempStr = detail::format_toMax5length(maxV + (stepSize / 2));
        for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
            self.label_horBottom.push_back(Config::space);
        }
        self.label_horBottom.append(tempStr);
        self.label_horBottom.append(Config::term_setDefault);
    };

    auto [minV, maxV] = detail::compute_minMaxMulti(self.labelTS_data.value());
    computeLabels(minV, maxV);

    return self;
}
auto Scatter::compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    std::optional<std::vector<size_t>> opt_catIDs_vec = std::nullopt;
    if (dp.cat_colID.has_value()) {
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
                                                      self.values_data, opt_catIDs_vec, dp.color_basePalette);

    return self;
}

// ### END SCATTER ###


// MULTILINE

auto Multiline::compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_verRight = std::vector(self.areaHeight, std::string(" "));
    return self;
}

auto Multiline::compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    self.axis_horTop = std::vector(self.areaWidth, std::string(" "));
    return self;
}

auto Multiline::compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    self.plotArea = detail::BrailleDrawer::drawLines(self.areaWidth, self.areaHeight, self.labelTS_data.value(),
                                                     self.values_data, dp.color_basePalette);
    return self;
}

// ### END MULTILINE ###

} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom