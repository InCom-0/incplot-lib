#pragma once

#include <functional>
#include <limits>
#include <ranges>
#include <string>
#include <type_traits>
#include <variant>

#include <incplot/plot_structures.hpp>
#include <private/braille_drawer.hpp>


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

    using self_t      = std::remove_cvref_t<decltype(self)>;
    using expOfSelf_t = std::expected<self_t, incerr_c>;


    return std::bind_back(&self_t::template compute_descriptors<self_t>, dp, ds)(std::move(self))
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

        .and_then(std::bind_back(&self_t::template compute_plot_area<self_t>, dp, ds));
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
    for (size_t i = 0; i < areaHeight; ++i) {
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

    // Add padding on bottom
    for (int i = 0; i < pad_bottom; ++i) { result.push_back('\n'); }
    return result;
}
// ### END BASE ###

// BAR V
auto BarV::compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // VERTICAL LEFT LABELS SIZE
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
        auto &labelColVarRef = ds.vec_colVariants.at(dp.labelTS_colID.value());

        auto olset = [](auto const &var) -> size_t {
            if constexpr (std::same_as<std::decay_t<decltype(var)>, std::reference_wrapper<std::vector<std::string>>>) {
                return std::ranges::max(
                    std::views::transform(var.get(), [](auto const &a) { return detail::strlen_utf8(a); }));
            }
            else { return Config::max_sizeOfValueLabels; }
        };

        auto const maxLabelSize = std::visit(olset, labelColVarRef);

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
            typename decltype(ds.vec_colVariants)::value_type cat_values =
                std::get<1>(*std::ranges::find_if(std::views::enumerate(ds.vec_colVariants), [&](auto const &a) {
                    return std::get<0>(a) == dp.cat_colID.value();
                }));

            auto create_catIDs_vec = [&](auto const &vec) -> std::vector<std::string> {
                auto sortedUniqued = detail::get_sortedAndUniqued(vec.get());
                if constexpr (std::same_as<typename decltype(sortedUniqued)::value_type, std::string>) {
                    return sortedUniqued;
                }
                else {
                    std::vector<std::string> res;
                    for (auto const &suItem : sortedUniqued) { res.push_back(std::to_string(suItem)); }
                    return res;
                }
            };
            auto catIDs_vec = std::visit(create_catIDs_vec, cat_values);

            size_t maxSize =
                std::ranges::max(std::views::transform(catIDs_vec, [](auto const &a) { return a.size(); }));
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
        // Else if more than one YvalCols, legend will be column names of those YvalCols
        else if (dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : dp.values_colIDs) { maxSize = std::max(maxSize, ds.colNames.at(colID).size()); }
            self.labels_verRightWidth = std::min(maxSize, Config::axisLabels_maxLength_vr);
        }
        else { self.labels_verRightWidth = 0; }
    }
    else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
        if (dp.values_colIDs.size() > 1) {
            size_t maxSize = 0;
            for (auto const &colID : dp.values_colIDs) { maxSize = std::max(maxSize, ds.colNames.at(colID).size()); }
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
    if (self.areaWidth < Config::min_areaWidth) {
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
        self.areaHeight =
            std::visit([](auto const &a) { return a.get().size(); }, ds.vec_colVariants.at(dp.labelTS_colID.value()));
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
    if (self.areaHeight < Config::min_areaHeight) {
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
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.labelTS_colID.value()), self.areaHeight);
        }
        else {
            self.axisName_verLeft =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.at(0)), self.areaHeight);
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

    self.labels_verLeft.push_back(
        std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

    auto const &labelsRef = ds.vec_colVariants.at(dp.labelTS_colID.value());
    auto        olset     = [&](auto const &vari) -> void {
        if constexpr (std::same_as<std::decay_t<decltype(vari)>, std::reference_wrapper<std::vector<std::string>>>) {
            for (auto const &rawLabel : vari.get()) {
                self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth));
                for (int i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                    self.labels_verLeft.back().push_back(Config::space);
                }
            }
        }
        else {
            for (auto const &rawLabelNumeric : vari.get()) {
                self.labels_verLeft.push_back(
                    detail::trim2Size_leading(detail::format_toMax5length(rawLabelNumeric), self.labels_verLeftWidth));
                for (int i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                    self.labels_verLeft.back().push_back(Config::space);
                }
            }
        }
    };

    std::visit(olset, labelsRef);

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
// All corners are simply empty as default ... but can possibly be used for something later if overrided in
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
    self.axisName_horBottom = detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.at(0)), self.areaWidth);
    return self;
}
auto BarV::compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computeLabels = [&](auto const &valColRef) -> void {
        size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
        auto const [minV, maxV] = std::ranges::minmax(valColRef);
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
    };
    if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
        self.label_horBottom = std::string(self.areaWidth + 2, Config::space);
    }
    else {
        auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef.first == parsedVal_t::double_like) {
            auto const &valColRef = ds.doubleCols.at(valColTypeRef.second);
            computeLabels(valColRef);
        }
        else {
            auto const &valColRef = ds.llCols.at(valColTypeRef.second);
            computeLabels(valColRef);
        }
    }
    return self;
}


auto BarV::compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto computePA = [&]<typename T>(T const &valColRef) -> void {
        auto const [minV, maxV] = std::ranges::minmax(valColRef);
        long long scalingFactor;
        if constexpr (std::is_integral_v<std::decay_t<typename T::value_type>>) {
            scalingFactor = std::numeric_limits<long long>::max() / (std::max(std::abs(maxV), std::abs(minV)));
        }
        else if constexpr (std::is_floating_point_v<std::decay_t<typename T::value_type>>) { scalingFactor = 1; }
        else { static_assert(false); } // Can't plot non-numeric values

        auto maxV_adj = maxV * scalingFactor;
        auto minV_adj = minV * scalingFactor;
        auto stepSize = (maxV_adj - minV_adj) / (self.areaWidth);

        for (auto const &val : valColRef) {
            long long rpt = (val * scalingFactor - minV_adj) / stepSize;
            self.plotArea.push_back(TermColors::get_basicColor(dp.color_basePalette.front()));
            for (long long i = rpt; i > 0; --i) { self.plotArea.back().append("■"); }
            self.plotArea.back().append(Config::color_Axes);

            self.plotArea.back().append(Config::term_setDefault);
            for (long long i = rpt; i < self.areaWidth; ++i) { self.plotArea.back().push_back(Config::space); }
        }
    };

    auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
    if (valColTypeRef.first == parsedVal_t::double_like) { computePA(ds.doubleCols.at(valColTypeRef.second)); }
    else { computePA(ds.llCols.at(valColTypeRef.second)); }
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
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.at(0)), self.areaHeight);
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

        for (int id = 0; id < self.axis_verLeftSteps; ++id) {
            for (int fillID = 0; fillID < fillerLength; ++fillID) {
                res.at(id * (fillerLength + 1) + fillID + 1).append(filler);
            }
            // Value label at the current position
            res.at(id * (fillerLength + 1) + fillerLength + 1)
                .append(detail::trim2Size_leading(
                    detail::format_toMax5length(minVal + (stepSize * (id + 1) * (fillerLength + 1))), labelsWidth));
        }

        // Filler up to 'max point'
        for (int i = self.axis_verLeftSteps * (fillerLength + 1) + 1; i < res.size() - 1; ++i) {
            res.at(i).append(filler);
        }

        // Value label of 'max point'
        res.back().append(detail::trim2Size_leading(detail::format_toMax5length(maxVal), labelsWidth));
        for (auto &line : res) {
            for (int i = 0; i < padRight; ++i) { line.push_back(Config::space); }
        }
        std::ranges::reverse(res);
        return res;
    };

    auto yValCols_fv =
        std::views::filter(std::views::enumerate(ds.vec_colVariants),
                           [&](auto const &pr) {
                               return (std::ranges::find(dp.values_colIDs, std::get<0>(pr)) != dp.values_colIDs.end());
                           }) |
        std::views::transform([](auto const &pr2) { return std::get<1>(pr2); });

    auto [minV, maxV] = detail::compute_minMaxMulti(yValCols_fv);
    self.labels_verLeft =
        getValLabels(minV, maxV, self.areaHeight, self.labels_verLeftWidth, Config::axisLabels_padRight_vl, 0);

    return (self);
}
// labels_vr are actually the legend here
auto Scatter::compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    // Categories are specified by catColID
    if (dp.cat_colID.has_value()) {
        typename decltype(ds.vec_colVariants)::value_type cat_values =
            std::get<1>(*std::ranges::find_if(std::views::enumerate(ds.vec_colVariants),
                                              [&](auto const &a) { return std::get<0>(a) == dp.cat_colID.value(); }));

        auto create_catIDs_vec = [&](auto const &vec) -> std::vector<std::string> {
            auto sortedUniqued = detail::get_sortedAndUniqued(vec.get());
            if constexpr (std::same_as<typename decltype(sortedUniqued)::value_type, std::string>) {
                return sortedUniqued;
            }
            else {
                std::vector<std::string> res;
                for (auto const &suItem : sortedUniqued) { res.push_back(std::to_string(suItem)); }
                return res;
            }
        };
        auto uniquedCats_vec = std::visit(create_catIDs_vec, cat_values);
        // horTop axis line
        self.labels_verRight.push_back(
            std::string(self.labels_verRightWidth + Config::axisLabels_padLeft_vr, Config::space));

        for (size_t lineID = 0; lineID < self.areaHeight; ++lineID) {
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

        for (size_t lineID = 0; lineID < self.areaHeight; ++lineID) {
            if (lineID < (dp.values_colIDs.size())) {
                self.labels_verRight.push_back(
                    std::string(Config::axisLabels_padLeft_vr, Config::space)
                        .append(TermColors::get_basicColor(dp.color_basePalette.at(lineID)))
                        .append(detail::trim2Size_ending(ds.colNames.at(dp.values_colIDs.at(lineID)),
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
    for (size_t i = 0; i < self.areaWidth; ++i) { self.axis_horTop.push_back(Config::axisFiller_t); }
    return self;
}

auto Scatter::compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
    // Name of the TS column
    self.axisName_horBottom = detail::trim2Size_leadingEnding(ds.colNames.at(dp.labelTS_colID.value()), self.areaWidth);
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

    auto [minV, maxV] = detail::compute_minMaxMulti(std::vector{ds.vec_colVariants.at(dp.labelTS_colID.value())});
    computeLabels(minV, maxV);

    return self;
}
auto Scatter::compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {

    auto const &valColTypeRef_x = ds.colTypes.at(dp.labelTS_colID.value());

    // Filter only the yValCols, also filter out the first selected column
    auto view_yValCols = std::views::enumerate(ds.vec_colVariants) | std::views::filter([&](auto const &a) {
                             return (std::ranges::find(dp.values_colIDs, std::get<0>(a)) != dp.values_colIDs.end());
                         }) |
                         std::views::transform([](auto const &b) { return std::get<1>(b); });

    std::optional<std::vector<size_t>> opt_catIDs_vec = std::nullopt;

    if (dp.cat_colID.has_value()) {
        typename decltype(ds.vec_colVariants)::value_type cat_values =
            std::get<1>(*std::ranges::find_if(std::views::enumerate(ds.vec_colVariants),
                                              [&](auto const &a) { return std::get<0>(a) == dp.cat_colID.value(); }));

        auto create_catIDs_vec = [&](auto const &vec) -> std::vector<size_t> {
            auto                catVals_uniqued = detail::get_sortedAndUniqued(vec.get());
            std::vector<size_t> catIDs_vec;

            for (auto const &catVal : vec.get()) {
                // Push_back catValue's designated ID into a dedicated vector
                catIDs_vec.push_back(std::ranges::find_if(catVals_uniqued, [&](auto const &a) { return a == catVal; }) -
                                     catVals_uniqued.begin());
            }
            return catIDs_vec;
        };

        opt_catIDs_vec = std::visit(create_catIDs_vec, cat_values);
    }

    if (valColTypeRef_x.first == parsedVal_t::double_like) {
        self.plotArea =
            detail::BrailleDrawer::drawPoints(self.areaWidth, self.areaHeight, ds.doubleCols.at(valColTypeRef_x.second),
                                              view_yValCols, opt_catIDs_vec, dp.color_basePalette);
    }
    else {
        self.plotArea =
            detail::BrailleDrawer::drawPoints(self.areaWidth, self.areaHeight, ds.llCols.at(valColTypeRef_x.second),
                                              view_yValCols, opt_catIDs_vec, dp.color_basePalette);
    }


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

    auto const &valColTypeRef_x = ds.colTypes.at(dp.labelTS_colID.value());

    // Filter only the yValCols, all the valueCols are on Y axis (X axis is the TS col)
    auto view_yValCols = std::views::enumerate(ds.vec_colVariants) | std::views::filter([&](auto const &a) {
                             return (std::ranges::find(dp.values_colIDs, std::get<0>(a)) != dp.values_colIDs.end());
                         }) |
                         std::views::transform([](auto const &b) { return std::get<1>(b); });

    if (valColTypeRef_x.first == parsedVal_t::double_like) {
        self.plotArea =
            detail::BrailleDrawer::drawLines(self.areaWidth, self.areaHeight, ds.doubleCols.at(valColTypeRef_x.second),
                                             view_yValCols, dp.color_basePalette);
    }
    else {
        self.plotArea = detail::BrailleDrawer::drawLines(
            self.areaWidth, self.areaHeight, ds.llCols.at(valColTypeRef_x.second), view_yValCols, dp.color_basePalette);
    }


    return self;
}

// ### END MULTILINE ###

} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom