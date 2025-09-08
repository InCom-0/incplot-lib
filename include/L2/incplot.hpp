#pragma once

#include <incplot/args.hpp>
#include <incplot/parsers_inc.hpp>
#include <incplot/plot_structures.hpp>
#include <variant>


namespace incom {
namespace terminal_plot {

// This is the only place where one 'selects' the plot types (ie. 'plot_structures') that will be used by the library.
using var_plotTypes = std::variant<plot_structures::BarV, plot_structures::BarVM, plot_structures::Multiline,
                                   plot_structures::Scatter, plot_structures::BarHM, plot_structures::BarHS>;

using incerr_c = incerr::incerr_code;
using enum Unexp_plotDrawer;

// MAIN SIMPLIFIED INTERFACE OF THE LIBRARY


std::expected<std::string, incerr_c>        make_plot(DesiredPlot &&dp_ctrs, std::string_view inputData);
inline std::expected<std::string, incerr_c> make_plot(DesiredPlot const &dp_ctrs, std::string_view inputData) {
    return make_plot(DesiredPlot(dp_ctrs), inputData);
}

std::string        make_plot_collapseUnExp(DesiredPlot &&dp_ctrs, std::string_view inputData);
inline std::string make_plot_collapseUnExp(DesiredPlot const &dp_ctrs, std::string_view inputData) {
    return make_plot_collapseUnExp(DesiredPlot(dp_ctrs), inputData);
}

std::expected<var_plotTypes, incerr_c> build_plotStructure(DesiredPlot const &dp, DataStore const &ds);

std::expected<DesiredPlot, incerr_c>        evaluate_onePSpossibility(DesiredPlot const &dp, DataStore const &ds);
inline std::expected<DesiredPlot, incerr_c> evaluate_onePSpossibility(DesiredPlot &&dp, DataStore const &ds) {
    return evaluate_onePSpossibility(dp, ds);
}

std::expected<DesiredPlot, incerr_c> evaluate_allPSpossibilities(DesiredPlot const &dp, DataStore const &ds);
inline std::expected<DesiredPlot, incerr_c> evaluate_allPSpossibilities(DesiredPlot &&dp, DataStore const &ds) {
    return evaluate_allPSpossibilities(dp, ds);
}


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;