#pragma once

#include <incplot/args.hpp>
#include <incplot/parsers_inc.hpp>
#include <incplot/plot_structures.hpp>
#include <variant>


namespace incom {
namespace terminal_plot {

// This is the only place where one 'selects' the plot types (ie. 'plot_structures') that will be used by the library.
using var_plotTypes =
    std::variant<plot_structures::BarV, plot_structures::BarH, plot_structures::Multiline, plot_structures::Scatter>;

template <typename PS_VAR>
class PlotDrawer {
private:
    PS_VAR m_ps_var;

    // CONSTRUCTION
    // Is private on purpose, gotta go through make_plotDrawer
    constexpr PlotDrawer() {};
    PlotDrawer(auto ps_var) : m_ps_var(std::move(ps_var)) {}

public:
    friend auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds)
        -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer>;

    // MAIN METHODS
    bool        validate_self() const;
    std::string drawPlot() const;

    std::expected<std::string, Unexp_plotDrawer> validateAndDrawPlot() const;
};

auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer>;

// MAIN SIMPLIFIED INTERFACE OF THE LIBRARY
std::expected<std::string, std::string> make_plot(DesiredPlot::DP_CtorStruct const &dp_ctrs,
                                                  std::string_view                  inputData);
std::expected<std::string, std::string> make_plot(DesiredPlot::DP_CtorStruct const &&dp_ctrs,
                                                  std::string_view                   inputData);

std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData);
std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData);


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;