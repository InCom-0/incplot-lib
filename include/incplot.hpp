#pragma once

#include <incplot/args.hpp>
#include <incplot/cross_platform.hpp>
#include <incplot/parsers_inc.hpp>
#include <incplot/plot_structures.hpp>
#include <variant>


namespace incom {
namespace terminal_plot {

using var_plotTypes =
    std::variant<plot_structures::BarV, plot_structures::BarH, plot_structures::Multiline, plot_structures::Scatter>;

template <typename PS_VAR>
class PlotDrawer {
private:
    PS_VAR m_ps_var;

public:
    // CONSTRUCTION
    constexpr PlotDrawer() {};
    PlotDrawer(auto ps_var) : m_ps_var(std::move(ps_var)) {}

    // MAIN METHODS
    bool        validate_self() const;
    std::string drawPlot() const;

    std::expected<std::string, Unexp_plotDrawer> validateAndDrawPlot() const;
};

auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer>;

} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;