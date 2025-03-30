#pragma once

#include <concepts>
#include <expected>

#include <functional>
#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <incplot/detail/concepts.hpp>
#include <incplot/detail/misc.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>


namespace incom {
namespace terminal_plot {
// FORWARD DELCARATIONS
namespace plot_structures {
class Base;
class BarV;
class BarH;
class Line;
class Multiline;
class Scatter;
class Bubble;
} // namespace plot_structures

// FORWARD DELCARATIONS --- END
// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'


class DesiredPlot {
    using NLMjson = nlohmann::json;

private:
    static std::expected<DesiredPlot, Unexp_plotSpecs> transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                  DataStore const &ds) {
        if (dp.label_colName.has_value()) {
            auto it = std::ranges::find(ds.colNames, dp.label_colName.value());
            if (it == ds.colNames.end()) { return std::unexpected(Unexp_plotSpecs::namesIntoIDs_label); }
            else if (not dp.label_colID.has_value()) { dp.label_colID = it - ds.colNames.begin(); }
            else if ((it - ds.colNames.begin()) == dp.label_colID.value()) { dp.label_colName = std::nullopt; }
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
        auto   valColTypeRng = std::views::filter(ds.colTypes, [](auto &&a) {
            return (a.first == NLMjson::value_t::number_float || a.first == NLMjson::value_t::number_integer ||
                    a.first == NLMjson::value_t::number_unsigned);
        });
        size_t valCols_sz    = std::ranges::count_if(valColTypeRng, [](auto &&a) { return true; });

        auto labelColTypeRng =
            std::views::filter(ds.colTypes, [](auto &&a) { return (a.first == NLMjson::value_t::string); });
        size_t labelCols_sz = std::ranges::count_if(labelColTypeRng, [](auto &&a) { return true; });

        // Actual decision making
        if (valCols_sz > Config::max_numOfValCols) { return std::unexpected(Unexp_plotSpecs::valCols); }
        // BarV
        else if (valCols_sz == 1 && labelCols_sz > 0) {
            dp.plot_type_name = detail::TypeToString<plot_structures::BarV>();
        }
        // Bubble ? Maybe later ... correction: probably never because it sucks visually :-)
        // else if (valCols_sz == 3) { dp.plot_type_name = detail::TypeToString<plot_structures::Bubble>(); }

        // Scatter
        else if (valCols_sz > 1 && labelCols_sz == 0) {
            dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>();
        }
        // Single line
        else if (valCols_sz == 1) { dp.plot_type_name = detail::TypeToString<plot_structures::Line>(); }
        else { dp.plot_type_name = detail::TypeToString<plot_structures::Multiline>(); }

        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_labelCol(DesiredPlot &&dp, DataStore const &ds) {
        if (dp.label_colID.has_value()) { return dp; }
        else if (dp.plot_type_name != detail::TypeToString<plot_structures::BarV>()) { return dp; }
        else {
            auto it = std::ranges::find_if(ds.colTypes, [](auto &&a) { return a.first == NLMjson::value_t::string; });
            if (it == ds.colTypes.end()) { return std::unexpected(Unexp_plotSpecs::labelCol); }
            else { dp.label_colID = it->second; }
            return dp;
        }
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_valueCols(DesiredPlot &&dp, DataStore const &ds) {
        auto addValColsUntil = [&](size_t count) -> std::expected<size_t, Unexp_plotSpecs> {
            auto getAnotherValColID = [&]() -> std::expected<size_t, Unexp_plotSpecs> {
                for (size_t i = 0; i < ds.colTypes.size(); ++i) {
                    if (ds.colTypes[i].first == NLMjson::value_t::number_float ||
                        ds.colTypes[i].first == NLMjson::value_t::number_integer ||
                        ds.colTypes[i].first == NLMjson::value_t::number_unsigned) {
                        if (std::ranges::find(dp.values_colIDs, i) == dp.values_colIDs.end()) { return i; }
                    }
                }
                // Cannot find another one
                return std::unexpected(Unexp_plotSpecs::guessValCols);
            };
            while (dp.values_colIDs.size() < count) {
                auto expID = getAnotherValColID();
                if (expID.has_value()) { dp.values_colIDs.push_back(expID.value()); }
                else { return std::unexpected(expID.error()); }
            }
            return 0uz;
        };

        // BAR PLOTS
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        // LINE PLOTS
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Line>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
            if (dp.values_colIDs.size() > 4) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(4).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        // BUBBLE PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Bubble>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Unexp_plotSpecs::valCols); }
            else if (not addValColsUntil(3).has_value()) { return std::unexpected(Unexp_plotSpecs::guessValCols); }
        }
        return dp;
    }
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_sizes(DesiredPlot &&dp, DataStore const &ds) {
        // Width always need to be provided, otherwise the whole thing doesn't work
        if (not dp.targetWidth.has_value() || dp.targetWidth.value() < 16) {
            return std::unexpected(Unexp_plotSpecs::tarWidth);
        }

        // Height can be inferred
        if (not dp.targetHeight.has_value()) { dp.targetHeight = dp.targetWidth.value() / 2; }

        // Impossible to print with height <3 under all circumstances
        if (dp.targetHeight.value() < 3) { return std::unexpected(Unexp_plotSpecs::tarWidth); }


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
    // Column data assessment for the purpose of DesiredPlot decision making


public:
    std::optional<std::string> plot_type_name;

    std::optional<size_t>      label_colID; // ID in colTypes
    std::optional<std::string> label_colName;

    // TODO: Make both 'values_' into std::optional as well to keep the logic the same for all here
    std::vector<size_t>      values_colIDs; // IDs in colTypes
    std::vector<std::string> values_colNames;

    std::optional<size_t> targetHeight;
    std::optional<size_t> targetWidth;

    std::optional<bool> valAxesNames_bool;
    std::optional<bool> valAxesLabels_bool;
    std::optional<bool> valAutoFormat_bool;
    std::optional<bool> legend_bool;


    // TODO: Provide some compile time programmatic way to set the default sizes here
    DesiredPlot(std::optional<size_t> tar_width = std::nullopt, std::optional<size_t> tar_height = std::nullopt,
                std::optional<std::string> plot_type_name = std::nullopt, std::optional<size_t> l_colID = std::nullopt,
                std::optional<std::string> l_colName = std::nullopt, std::vector<size_t> v_colIDs = {},
                std::vector<std::string> v_colNames = {})
        : targetWidth(tar_width), targetHeight(tar_height), plot_type_name(std::move(plot_type_name)),
          label_colID(std::move(l_colID)), label_colName(std::move(l_colName)), values_colIDs(std::move(v_colIDs)),
          values_colNames(std::move(v_colNames)) {}

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

        auto gpt = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_plotType(std::forward<decltype(dp)>(dp), ds);
        };
        auto glc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Unexp_plotSpecs> {
            return DesiredPlot::guess_labelCol(std::forward<decltype(dp)>(dp), ds);
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

        return DesiredPlot::transform_namedColsIntoIDs(std::forward<decltype(self)>(self), ds)
            .and_then(gpt)
            .and_then(glc)
            .and_then(gvc)
            .and_then(gsz)
            .and_then(gtff);
    }
};

struct PlotDataWrapper {
public:
    std::vector<std::variant<std::reference_wrapper<const std::vector<long long>>,
                             std::reference_wrapper<const std::vector<double>>>>
        m_varVect;

    void push_back(auto &oneVect) { m_varVect.push_back(std::ref(oneVect)); }

    PlotDataWrapper(DesiredPlot const &dp, DataStore const &ds, size_t startWith_valColID)
        : PlotDataWrapper(dp, ds,
                          std::views::iota(startWith_valColID, dp.values_colIDs.size()) |
                              std::ranges::to<std::vector>()) {}

    PlotDataWrapper(DesiredPlot const &dp, DataStore const &ds, std::vector<size_t> const &&valColIDs) {
        for (auto const &vcID : valColIDs) {

            if (ds.colTypes.at(dp.values_colIDs.at(vcID)).first == nlohmann::detail::value_t::number_float) {
                m_varVect.push_back(std::ref(ds.doubleCols.at(ds.colTypes.at(dp.values_colIDs.at(vcID)).second)));
            }
            else { m_varVect.push_back(std::ref(ds.llCols.at(ds.colTypes.at(dp.values_colIDs.at(vcID)).second))); }
        }
    }

    std::pair<double, double> compute_minMax() const {
        std::pair<double, double> res{std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};
        for (auto const &variantRef : m_varVect) {
            auto ol_set = [&](auto const &var) -> void {
                auto [minV_l, maxV_l] = std::ranges::minmax(var.get());
                res.first             = std::min(res.first, static_cast<double>(minV_l));
                res.second            = std::max(res.second, static_cast<double>(maxV_l));
            };
            std::visit(ol_set, variantRef);
        }
        return res;
    }
};

} // namespace terminal_plot
} // namespace incom
