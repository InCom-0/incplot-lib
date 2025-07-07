#pragma once

#include <incplot/desired_plot.hpp>
#include <vector>


namespace incom {
namespace terminal_plot {
namespace plot_structures {

using incerr_c = incerr::incerr_code;

// Classes derived from base represent 'plot structures' of particular types of plots (such as bar vertical, scatter
// etc.)
// Create your own 'plot structure' ie. type of plot by deriving from 'Base' class (or from other classes derived
// from it). 'Shadow' the 'Compute_*' methods as needed as if you were overriding (but without override keyword). The
// types properly derived from 'Base' can then be used inside std::variant<...>.
// The idea is to be able to easily customize and also possibly 'partially customize' as needed.
// You always have to make the 'Base' class a friend ... this enables really nice static compile-time polymorphism
// coupled with 'deducing this' feature of C++23
class Base {
protected:
    using vec_val_t =
        decltype(std::declval<DataStore::Column &>().get_filteredVariantData(std::vector<unsigned char>()));

    std::vector<vec_val_t> labelTS_colView;
    std::vector<vec_val_t> cat_colView;
    std::vector<vec_val_t> values_colViews;

public:
    // Descriptors - First thing to be computed.
    // BEWARE: The sizes here are 'as displayed' not the 'size in bytes' ... need to account for UTF8
    long long areaWidth = 0, areaHeight = 0;
    size_t    labels_verLeftWidth = 0, labels_verRightWidth = 0;


    size_t axis_verLeftSteps = 0, axis_varRightSteps = 0, axis_horTopSteps = 0, axis_horBottomSteps = 0;

    long long pad_left = Config::ps_padLeft, pad_right = Config::ps_padRight, pad_top = Config::ps_padTop,
              pad_bottom = Config::ps_padBottom;

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

    std::string areaCorner_tl;
    std::string areaCorner_bl;
    std::string areaCorner_br;
    std::string areaCorner_tr;

    std::vector<std::string> plotArea;

    // Compute size in bytes (for reserving the output str size), not size in 'displayed characters'
    size_t compute_lengthOfSelf() const;

public:
    // This needs to get called after default construction
    auto build_self(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    // TODO: Implement 'valiate_self()' ... consider if it is even needed or if its not already done elsewhere
    bool        validate_self() const { return true; }
    std::string build_plotAsString() const;

private:
    // TODO: Implement validate_descriptors for 'plot_structures'
    auto validate_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
        return self;
    }

    // One needs to define all of these in a derived class.
    // All 'Compute_*' methods are deleted in Base class on purpose to make code not compile if you don't provide
    // implementation in some derived class
    auto initialize_data_views(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_areaCorners(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
};

class BarV : public Base {
    friend class Base;

private:
    auto initialize_data_views(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_descriptors(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axis_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_corner_tl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_bl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_tr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_br(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_areaCorners(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class BarH : public BarV {
    friend class Base;
};

class Scatter : public BarV {
    friend class Base;

private:
    auto compute_axisName_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vl(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    // labels_vr are actually the legend here
    auto compute_labels_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axisName_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_hb(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class Multiline : public Scatter {
    friend class Base;

private:
    auto compute_axis_vr(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axis_ht(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_plot_area(this auto &&self, DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom