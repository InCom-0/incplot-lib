#pragma once

#include <expected>
#include <functional>
#include <incplot/datastore.hpp>
#include <incplot/desired_plot.hpp>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>


namespace incom {
namespace terminal_plot {
namespace plot_structures {

using incerr_c             = incerr::incerr_code;
using guess_firstParamType = std::pair<std::reference_wrapper<DesiredPlot>, size_t>;
using guess_retType        = std::expected<guess_firstParamType, incerr_c>;

// Forward declaration
class Base;

template <typename PS>
requires(std::is_base_of_v<Base, PS>)
std::expected<std::pair<DesiredPlot, size_t>, incerr_c> evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds) {

    using enum incom::terminal_plot::Unexp_plotSpecs;
    auto isNot_differentPS = [&](guess_firstParamType &&dp_pr, DataStore const &ds) -> guess_retType {
        if (dp_pr.first.get().plot_type_name.has_value()) {
            if (dp_pr.first.get().plot_type_name.value() != std::type_index(typeid(PS))) {
                return std::unexpected(incerr_c::make(GPT_explicitlySpecifiedDifferentPlotType));
            }
        }
        return dp_pr;
    };
    auto c_fflags = [&](guess_firstParamType &&dp_pr) -> guess_retType {
        DesiredPlot::compute_filterFlags_r_void(dp_pr.first.get(), ds);
        return dp_pr;
    };

    auto res = isNot_differentPS(guess_firstParamType{std::ref(dp), 0uz}, ds)
                   .and_then(std::bind_back(PS::guess_TSCol, ds))
                   .and_then(std::bind_back(PS::guess_catCol, ds))
                   .and_then(std::bind_back(PS::guess_valueCols, ds))
                   .and_then(c_fflags)
                   .and_then(std::bind_back(PS::guess_sizes, ds))
                   .and_then(std::bind_back(PS::guess_TFfeatures, ds));

    if (res.has_value()) { return std::make_pair(std::move(res.value().first.get()), res.value().second); }
    else { return std::unexpected(res.error()); }
}

template <typename... PSs>
requires(std::is_base_of_v<Base, PSs>, ...) && (sizeof...(PSs) > 0)
auto evaluate_PSs_asPossibilities(DesiredPlot dp, DataStore const &ds) {
    auto dp_exp = DesiredPlot::compute_colAssessments(std::move(dp), ds)
                      .and_then(std::bind_back(DesiredPlot::transform_namedColsIntoIDs, ds));

    std::array<std::pair<std::type_index, std::expected<std::pair<DesiredPlot, size_t>, incerr_c>>, sizeof...(PSs)> res{
        {{std::type_index(typeid(PSs)), evaluate_PS_asPossibility<PSs>(dp_exp.value(), ds)}...}};
    return res;
}

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
    // LEGACY WAY TO ACCESS DATA ... local views into the data held in DataStore

    // using vec_val_t =
    //     decltype(std::declval<DataStore::Column &>().get_filteredVariantData(std::vector<unsigned int>()));

    // std::optional<vec_val_t> labelTS_dataView = std::nullopt;
    // std::optional<vec_val_t> LOC_cat_dataView     = std::nullopt;
    // std::vector<vec_val_t>   LOC_values_dataViews;+

    DataStore const   &ds;
    DesiredPlot const &dp;

    // NEW WAY TO ACCESS DATA ... local copies of the data in question
    std::optional<DataStore::varCol_t> labelTS_data = std::nullopt;
    std::optional<DataStore::varCol_t> cat_data     = std::nullopt;
    std::vector<DataStore::varCol_t>   values_data;
    size_t                             data_rowCount = std::numeric_limits<size_t>::max();

public:
    // Descriptors - First thing to be computed.
    // BEWARE: The sizes here are 'as displayed' not the 'size in bytes' ... need to account for UTF8
    long long areaWidth = 0, areaHeight = 0;
    size_t    labels_verLeftWidth = 0, labels_verRightWidth = 0;


    size_t axis_verLeftSteps = 0, axis_varRightSteps = 0, axis_horTopSteps = 0, axis_horBottomSteps = 0;

    long long pad_left = Config::ps_padLeft, pad_right = Config::ps_padRight, pad_top = Config::ps_padTop,
              pad_bottom = Config::ps_padBottom;

    bool axisName_horTop_bool = false, axisName_horBottom_bool = false;

    bool axisName_verLeft_bool = false, axisName_verRight_bool = false;


    // Actual structure
    std::string              axisName_verLeft = "";
    std::vector<std::string> labels_verLeft;
    std::vector<std::string> axis_verLeft;

    std::vector<std::string> axis_verRight;
    std::vector<std::string> labels_verRight;
    std::string              axisName_verRight = "";

    std::string              axisName_horTop = "";
    std::vector<std::string> labels_horTop;
    std::vector<std::string> axis_horTop;

    std::vector<std::string> axis_horBottom;
    std::vector<std::string> labels_horBottom;
    std::string              axisName_horBottom = "";

    std::vector<std::string> corner_topLeft;
    std::vector<std::string> corner_bottomLeft;
    std::vector<std::string> corner_bottomRight;
    std::vector<std::string> corner_topRight;

    std::string areaCorner_tl = "";
    std::string areaCorner_bl = "";
    std::string areaCorner_br = "";
    std::string areaCorner_tr = "";

    std::vector<std::string> plotArea;

    std::string footer = "";

    // Compute size in bytes (for reserving the output str size), not size in 'displayed characters'
    size_t compute_lengthOfSelf() const;

public:
    Base()             = delete;
    Base(Base const &) = default;
    Base(Base &)       = default;
    Base(Base &&)      = default;
    ~Base()            = default;

    Base(DesiredPlot const &dp_ext, DataStore const &ds_ext) : dp(dp_ext), ds(ds_ext) {};

    // This needs to get called after default construction
    auto build_self(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    // TODO: Implement 'valiate_self()' ... consider if it is even needed or if its not already done elsewhere
    bool        validate_self() const { return true; }
    std::string build_plotAsString() const;

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds)      = delete;
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds)     = delete;
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds)  = delete;
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds)      = delete;
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds) = delete;

protected:
    // TODO: Implement validate_descriptors for 'plot_structures'
    auto validate_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> {
        return self;
    }

    // One needs to define all of these in a derived class.
    // All 'Compute_*' methods are deleted in Base class on purpose to make code not compile if you don't provide
    // implementation in some derived class
    auto initialize_data_views(this auto &&self)
        -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axisName_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axis_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>     = delete;
    auto compute_axisName_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;

    auto compute_axis_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>     = delete;
    auto compute_axisName_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
    auto compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;

    auto compute_corner_tl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;
    auto compute_corner_bl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;
    auto compute_corner_br(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;
    auto compute_corner_tr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>   = delete;
    auto compute_areaCorners(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;

    auto compute_footer(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c> = delete;
};

class BarV : public Base {
    friend class Base;
    using Base::Base;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto initialize_data_views(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axis_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_corner_tl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_bl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_tr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_corner_br(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_areaCorners(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axisName_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_footer(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class BarVM : public BarV {
    friend class Base;
    using BarV::BarV;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};


class Scatter : public BarV {
    friend class Base;
    using BarV::BarV;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto compute_axisName_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    // labels_vr are actually the legend here
    auto compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_axisName_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;


    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class Multiline : public Scatter {
    friend class Base;
    using Scatter::Scatter;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto compute_axis_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axis_ht(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class BarHM : public Multiline {
    friend class Base;
    using Multiline::Multiline;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto compute_descriptors(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_axis_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_hb(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

class BarHS : public BarHM {
    friend class Base;
    using BarHM::BarHM;

    template <typename PS>
    requires(std::is_base_of_v<Base, PS>)
    friend std::expected<std::pair<DesiredPlot, size_t>, incerr_c> incom::terminal_plot::plot_structures::
        evaluate_PS_asPossibility(DesiredPlot dp, DataStore const &ds);

protected:
    static guess_retType guess_TSCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_catCol(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_valueCols(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_sizes(guess_firstParamType &&dp_pr, DataStore const &ds);
    static guess_retType guess_TFfeatures(guess_firstParamType &&dp_pr, DataStore const &ds);

protected:
    auto compute_labels_vl(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
    auto compute_labels_vr(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;

    auto compute_plot_area(this auto &&self) -> std::expected<std::remove_cvref_t<decltype(self)>, incerr_c>;
};

} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom