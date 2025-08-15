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

std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData);
std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData);

std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData);
std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData);

std::expected<var_plotTypes, incerr_c> build_plot_structure(DesiredPlot const &dp, DataStore const &ds);

} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;