#pragma once

#include "incplot/config.hpp"
#include "incplot/detail.hpp"
#include <algorithm>
#include <expected>
#include <limits>
#include <ranges>
#include <utility>

#include <incplot/datastore.hpp>
#include <incplot/desired_plot.hpp>
#include <incplot/detail.hpp>
#include <incplot/parser.hpp>
#include <vector>


namespace incom {
namespace terminal_plot {

namespace plot_structures {
// Classes derived from base represent 'plot structures' of particular types of plots (such as bar vertical, scatter
// etc.)
// Create your own 'plot structure' ie. type of plot by deriving from 'Base' class (or from other classes derived
// from it) and overriding pure virtual functions. The types properly derived from 'Base' can then be used inside
// 'PlotDrawer' inside std::variant<...>. The idea is to be able to easily customize and also possibly 'partially
// customize' as needed You always have to make the 'Base' class a friend ... this enables really nice static
// polymorphism coupled with 'deducing this' feature of C++23
class Base {
protected:
    // Descriptors - First thing to be computed.
    // BEWARE: The sizes here are 'as displayed' not the 'size in bytes' ... need to account for UTF8
    size_t areaWidth = 0, areaHeight = 0;
    size_t labels_verLeftWidth = 0, labels_verRightWidth = 0;


    size_t axis_verLeftSteps = 0, axis_varRightSteps = 0, axis_horTopSteps = 0, axis_horBottomSteps = 0;

    size_t pad_left = 2, pad_right = 0, pad_top = 1, pad_bottom = 0;

    bool labels_horTop_bool = false, labels_horBottom_bool = false;
    bool axisName_horTop_bool = false, axisName_horBottom_bool = false;

    bool axisName_verLeft_bool = false, axisName_verRight_bool = false;


    // Actual structure
    std::string              axisName_verLeft;
    std::vector<std::string> labels_verLeft;
    std::vector<std::string> axis_verLeft;

    std::string              axisName_verRight;
    std::vector<std::string> axis_verRight;
    std::vector<std::string> labels_verRight;

    std::string              axisName_horTop;
    std::string              label_horTop;
    std::vector<std::string> axis_horTop;

    std::vector<std::string> axis_horBottom;
    std::string              label_horBottom;
    std::string              axisName_horBottom;

    std::vector<std::string> corner_topLeft;
    std::vector<std::string> corner_bottomLeft;
    std::vector<std::string> corner_bottomRight;
    std::vector<std::string> corner_topRight;

    std::vector<std::string> plotArea;

    // Compute size in bytes (for reserving the output str size), not size in 'displayed characters'
    size_t compute_lengthOfSelf() const {

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

public:
    // This needs to get called after default construction
    auto build_self(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::remove_cvref_t<decltype(self)> {
        // Can only build it from rvalue ...
        if constexpr (std::is_lvalue_reference_v<decltype(self)>) { static_assert(false); }

        using expOfSelf_t = std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer>;

        auto c_dsc = [&](auto &&ps) -> expOfSelf_t { return ps.compute_descriptors(dp, ds); };
        auto v_dsc = [&](auto &&ps) -> expOfSelf_t { return ps.validate_descriptors(dp, ds); };

        auto c_anvl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_vl(dp, ds); };
        auto c_anvr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_vr(dp, ds); };

        auto c_lvl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_vl(dp, ds); };
        auto c_lvr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_vr(dp, ds); };

        auto c_avl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_vl(dp, ds); };
        auto c_avr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_vr(dp, ds); };

        auto c_ctl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_tl(dp, ds); };
        auto c_cbl = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_bl(dp, ds); };
        auto c_cbr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_br(dp, ds); };
        auto c_ctr = [&](auto &&ps) -> expOfSelf_t { return ps.compute_corner_tr(dp, ds); };

        auto c_aht  = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_ht(dp, ds); };
        auto c_anht = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_ht(dp, ds); };
        auto c_alht = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_ht(dp, ds); };

        auto c_ahb  = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axis_hb(dp, ds); };
        auto c_anhb = [&](auto &&ps) -> expOfSelf_t { return ps.compute_axisName_hb(dp, ds); };
        auto c_alhb = [&](auto &&ps) -> expOfSelf_t { return ps.compute_labels_hb(dp, ds); };

        auto c_ap = [&](auto &&ps) -> expOfSelf_t { return ps.compute_plot_area(dp, ds); };

        auto res = c_dsc(std::move(self))
                       .and_then(v_dsc)
                       .and_then(c_anvl)
                       .and_then(c_anvr)
                       .and_then(c_lvl)
                       .and_then(c_lvr)
                       .and_then(c_avl)
                       .and_then(c_avr)
                       .and_then(c_ctl)
                       .and_then(c_cbl)
                       .and_then(c_cbr)
                       .and_then(c_ctr)
                       .and_then(c_aht)
                       .and_then(c_anht)
                       .and_then(c_alht)
                       .and_then(c_ahb)
                       .and_then(c_anhb)
                       .and_then(c_alhb)
                       .and_then(c_ap);

        return res.value();
    }

    // TODO: Implement 'valiate_self()' ... consider if it is even needed or if its not already done elsewhere
    bool validate_self() const { return true; }


    std::string build_plotAsString() const {
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
        result.append("┌");
        for (auto const &toAppend : axis_horTop) { result.append(toAppend); }
        result.append(Config::color_Axes);
        result.append("┐");
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
        result.append("└");
        for (auto const &toAppend : axis_horBottom) { result.append(toAppend); }
        result.append(Config::color_Axes);
        result.append("┘");
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

private:
    // TODO: Implement validate_descriptors for 'plot_structures'
    auto validate_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }

    // One needs to define all of these in a derived class.
    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> = delete;
};

class BarV : public Base {
    friend class Base;

    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        // Vertical left labels size
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            auto const &labelColRef = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
            auto const  labelSizes =
                std::views::transform(labelColRef, [](auto const &a) { return detail::strlen_utf8(a); });

            self.labels_verLeftWidth = std::min(
                Config::axisLabels_maxLength_vl,
                std::min(std::ranges::max(labelSizes), (dp.targetWidth.value() - self.pad_left - self.pad_right) / 4));
        }
        else { self.labels_verLeftWidth = Config::max_valLabelSize; }

        // TODO: Vertical right labels ... probably nothing so keeping 0 size
        // ...

        // Vertical axes names ... LEFT always, RIGHT never
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axisName_verLeft_bool  = false;
            self.axisName_verRight_bool = false;
        }
        else {
            self.axisName_verLeft_bool  = true;
            self.axisName_verRight_bool = false;
        }

        // Plot area width (-2 is for the 2 vertical axes positions)
        self.areaWidth = dp.targetWidth.value() - self.pad_left -
                         (Config::axis_verName_width_vl * self.axisName_verLeft_bool) - self.labels_verLeftWidth - 2 -
                         self.labels_verRightWidth - (Config::axis_verName_width_vl * self.axisName_verRight_bool) -
                         self.pad_right;

        // Labels and axis name bottom
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>() ||
            (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) && true) {
        } // TODO: Proper assessment for Multiline
        else {
            self.labels_horBottom_bool   = true;
            self.axisName_horBottom_bool = true;
        }

        // Labels and axis name top ... probably nothing so keeping 0 size
        // ...

        // Plot area height (-2 is for the 2 horizontal axes positions)
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.areaHeight = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second).size();
        }
        else {
            self.areaHeight = dp.targetHeight.value() - self.pad_top - self.axisName_horTop_bool -
                              self.labels_horTop_bool - 2 - self.labels_horBottom_bool - self.axisName_horBottom_bool -
                              self.pad_bottom;
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

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_verLeft_bool) {
            if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
                self.axisName_verLeft =
                    detail::trim2Size_leadingEnding(ds.colNames.at(dp.label_colID.value()), self.areaHeight);
            }
            else {
                self.axisName_verLeft =
                    detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.front()), self.areaHeight);
            }
        }

        return self;
    }
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        auto const &labelsRef = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
        self.labels_verLeft.push_back(
            std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

        for (auto const &rawLabel : labelsRef) {
            self.labels_verLeft.push_back(detail::trim2Size_leading(rawLabel, self.labels_verLeftWidth));
            for (int i = 0; i < Config::axisLabels_padRight_vl; ++i) {
                self.labels_verLeft.back().push_back(Config::space);
            }
        }
        self.labels_verLeft.push_back(
            std::string(self.labels_verLeftWidth + Config::axisLabels_padRight_vl, Config::space));

        return (self);
    }
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        for (int i = 0; i < (self.areaHeight + 2); ++i) { self.labels_verRight.push_back(""); }
        return self;
    }

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            self.axis_verLeft = detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l, self.areaHeight,
                                                              self.areaHeight);
        }
        // All else should have vl axis ticks according to numeric values
        else {
            auto tmpAxis = detail::create_tickMarkedAxis(Config::axisFiller_l, Config::axisTick_l,
                                                         self.axis_verLeftSteps, self.areaHeight);
            std::ranges::reverse(tmpAxis);
            self.axis_verLeft = std::move(tmpAxis);
        }

        return self;
    }
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        self.axis_verRight = std::vector(self.areaHeight, std::string(" "));
        return self;
    }

    // All corners are simply empty as default ... but can possibly be used for something later if overrided in
    // derived
    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
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
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horBottom_bool) {
            self.corner_bottomLeft.push_back(
                std::string(self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                Config::axisLabels_padRight_vl,
                            Config::space));
        }
        if (self.labels_horBottom_bool) {
            self.corner_bottomLeft.push_back(
                std::string(self.labels_verLeftWidth + (Config::axis_verName_width_vl * self.axisName_verLeft_bool) +
                                Config::axisLabels_padRight_vl,
                            Config::space));
        }

        return self;
    }
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (self.axisName_horTop_bool) {
            self.corner_topRight.push_back(
                std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                Config::axisLabels_padLeft_vr,
                            Config::space));
        }
        if (self.labels_horTop_bool) {
            self.corner_topRight.push_back(
                std::string(self.labels_verRightWidth + (Config::axis_verName_width_vr * self.axisName_verRight_bool) +
                                Config::axisLabels_padLeft_vr,
                            Config::space));
        }

        return self;
    }
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
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

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        self.axis_horTop = std::vector(self.areaWidth, std::string(" "));
        return self;
    }
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        return self;
    }

    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
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
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            // Name of the LABEL column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.label_colID.value()), self.areaWidth);
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            // Name of the FIRST value column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.front()), self.areaWidth);
        }
        else {
            // Name of the SECOND value column
            self.axisName_horBottom =
                detail::trim2Size_leadingEnding(ds.colNames.at(dp.values_colIDs.at(1)), self.areaWidth);
        }
        return self;
    }
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computeLabels = [&](auto const &valColRef) -> void {
            size_t const fillerSize = detail::get_axisFillerSize(self.areaWidth, self.axis_horBottomSteps);
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            auto   stepSize         = (maxV - minV) / (self.areaWidth + 1);
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
            tempStr = detail::format_toMax5length(maxV);
            for (size_t i = 0; i < ((self.areaWidth + 2 - placedChars) - detail::strlen_utf8(tempStr)); ++i) {
                self.label_horBottom.push_back(Config::space);
            }
            self.label_horBottom.append(tempStr);
            self.label_horBottom.append(Config::term_setDefault);
        };
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.label_horBottom = std::string(self.areaWidth + 2, Config::space);
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            // TODO: What to do with Multiline axisLabel bottom
        }
        else {
            auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
            if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
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

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto computePA = [&]<typename T>(T const &valColRef) -> void {
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            long long scalingFactor;
            if constexpr (std::is_integral_v<std::decay_t<typename T::value_type>>) {
                scalingFactor = LONG_LONG_MAX / (std::max(std::abs(maxV), std::abs(minV)));
            }
            else if constexpr (std::is_floating_point_v<std::decay_t<typename T::value_type>>) { scalingFactor = 1; }
            else { static_assert(false); } // Can't plot non-numeric values

            auto maxV_adj = maxV * scalingFactor;
            auto minV_adj = minV * scalingFactor;
            auto stepSize = (maxV_adj - minV_adj) / (self.areaWidth + 1);

            for (auto const &val : valColRef) {
                long long rpt = (val * scalingFactor - minV_adj) / stepSize;
                self.plotArea.push_back(Config::color_Vals1);
                for (long long i = rpt; i > 0; --i) { self.plotArea.back().append("■"); }
                self.plotArea.back().append(Config::color_Axes);

                self.plotArea.back().append(Config::term_setDefault);
                for (long long i = rpt; i < self.areaWidth; ++i) { self.plotArea.back().push_back(Config::space); }
            }
        };

        auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
            computePA(ds.doubleCols.at(valColTypeRef.second));
        }
        else { computePA(ds.llCols.at(valColTypeRef.second)); }
        return self;
    }
};

class BarH : public BarV {
    friend class Base;
};

class Scatter : public BarV {
    friend class Base;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {


        auto getValLabels = [&](double const &minVal, double const &maxVal, size_t areaLength,
                                size_t const &labelsWidth, size_t const padRight, size_t const padLeft) {
            auto const  fillerLength = detail::get_axisFillerSize(areaLength, self.axis_verLeftSteps);
            std::string filler(labelsWidth, Config::space);
            auto        stepSize = (maxVal - minVal) / (areaLength + 1);

            // Construct with 'left padding' in place
            std::vector<std::string> res(areaLength + 2,
                                         std::string(padLeft, Config::space).append(Config::color_Axes));

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

        auto const &valColTypeRef_y = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef_y.first == nlohmann::detail::value_t::number_float) {
            auto [minV, maxV] = std::ranges::minmax(ds.doubleCols.at(valColTypeRef_y.second));
            self.labels_verLeft =
                getValLabels(minV, maxV, self.areaHeight, self.labels_verLeftWidth, Config::axisLabels_padRight_vl, 0);
        }
        else {
            auto [minV, maxV] = std::ranges::minmax(ds.llCols.at(valColTypeRef_y.second));
            self.labels_verLeft =
                getValLabels(minV, maxV, self.areaHeight, self.labels_verLeftWidth, Config::axisLabels_padRight_vl, 0);
        }

        return (self);
    }

    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        self.axis_verRight =
            detail::create_tickMarkedAxis(Config::axisFiller_r, Config::axisTick_r, 0, self.areaHeight);
        return self;
    }

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {
        for (size_t i = 0; i < self.areaWidth; ++i) { self.axis_horTop.push_back(Config::axisFiller_t); }
        return self;
    }

    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

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
                while (placedChars < (i * (fillerSize + 1) + fillerSize)) {
                    self.label_horBottom.push_back(Config::space);
                    placedChars++;
                }
                tempStr =
                    detail::format_toMax5length((minV - stepSize) + ((i * (fillerSize + 1) + fillerSize) * stepSize));
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
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            self.label_horBottom = std::string(self.areaWidth + 2, Config::space);
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            // TODO: What to do with Multiline axisLabel bottom
        }
        else {
            // The SECOND and ABOVE value columns are used for the minV maxV basis
            double minV = std::numeric_limits<double>::max(), maxV = std::numeric_limits<double>::min();

            for (size_t id = 1; id < dp.values_colIDs.size(); ++id) {
                if (ds.colTypes.at(id).first == nlohmann::detail::value_t::number_float) {
                    auto const [minV_tmp, maxV_tmp] =
                        std::ranges::minmax(ds.doubleCols.at(ds.colTypes.at(dp.values_colIDs.at(id)).second));
                    minV = std::min(minV, minV_tmp);
                    maxV = std::max(maxV, maxV_tmp);
                }
                else {
                    auto const [minV_tmp, maxV_tmp] =
                        std::ranges::minmax(ds.llCols.at(ds.colTypes.at(dp.values_colIDs.at(id)).second));
                    minV = std::min(minV, static_cast<double>(minV_tmp));
                    maxV = std::max(maxV, static_cast<double>(maxV_tmp));
                }
            }

            computeLabels(minV, maxV);
        }
        return self;
    }

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, Unexp_plotDrawer> {

        auto const &valColTypeRef_x = ds.colTypes.at(dp.values_colIDs.front());

        // Filter only the yValCols, also filter out the first selected column
        auto view_yValCols = std::views::enumerate(ds.vec_colVariants) | std::views::filter([&](auto const &a) {
                                 return (std::ranges::find(dp.values_colIDs.begin() + 1, dp.values_colIDs.end(),
                                                           std::get<0>(a)) != dp.values_colIDs.end());
                             }) |
                             std::views::transform([](auto const &b) { return std::get<1>(b); });

        if (valColTypeRef_x.first == nlohmann::detail::value_t::number_float) {
            self.plotArea = detail::BrailleDrawer::drawPoints(self.areaWidth, self.areaHeight,
                                                              ds.doubleCols.at(valColTypeRef_x.second), view_yValCols);
        }
        else {
            self.plotArea = detail::BrailleDrawer::drawPoints(self.areaWidth, self.areaHeight,
                                                              ds.llCols.at(valColTypeRef_x.second), view_yValCols);
        }

        return self;
    }
};

class Multiline : public Scatter {
    friend class Base;
};

} // namespace plot_structures


template <typename PS_VAR>
class PlotDrawer {
private:
    PS_VAR m_ps_var;

public:
    constexpr PlotDrawer() {};
    PlotDrawer(auto ps_var, DesiredPlot const &dp, DataStore const &ds) {
        auto ol = [&](auto &&var) {
            m_ps_var = std::move(ps_var).build_self(dp, ds);
            int a    = 0;
        };
        std::visit(ol, m_ps_var);
    }

    void update_newPlotStructure(DesiredPlot const &dp, DataStore const &ds) {
        auto ol = [&](auto &&var) { m_ps_var = decltype(var)().build_self(dp, ds); };
        std::visit(ol, m_ps_var);
    }

    bool validate_self() const {
        auto validate = [&](auto &&var) -> bool { return var.validate_self(); };
        return std::visit(validate, m_ps_var);
    }

    std::expected<std::string, Unexp_plotDrawer> validateAndDrawPlot() const {
        // TODO: Add some validation before drawing
        if (validate_self() == false) { return std::unexpected(Unexp_plotDrawer::plotStructureInvalid); }
        else { return drawPlot(); }
    }

    std::string drawPlot() const {
        auto ol = [&](auto &&var) -> std::string { return var.build_plotAsString(); };
        return std::visit(ol, m_ps_var);
    }
};

// This is a map of default constructed 'plot_structures' inside an std::variant
// Pass the 'plot_structure' template types that should be used by the library
// This is the only place where one 'selects' these template types
static inline const auto mp_names2Types =
    detail::generate_variantTypeMap<plot_structures::Base, plot_structures::BarV, plot_structures::BarH,
                                    plot_structures::Multiline, plot_structures::Scatter>();

inline auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds) {
    auto ref          = mp_names2Types.at(dp.plot_type_name.value());
    using varType     = decltype(ref);
    auto overload_set = [&](auto const &variantItem) -> PlotDrawer<varType> {
        return PlotDrawer<varType>(variantItem, dp, ds);
    };
    return std::visit(overload_set, ref);
}


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;