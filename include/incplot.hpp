#pragma once

#include <incplot/args.hpp>
#include <incplot/cross_platform.hpp>
#include <incplot/parser_inc.hpp>
#include <incplot/plot_structures.hpp>
#include <variant>


namespace incom {
namespace terminal_plot {

template <typename PS_VAR>
class PlotDrawer {
private:
    PS_VAR m_ps_var;

public:
    constexpr PlotDrawer() {};
    PlotDrawer(auto ps_var) : m_ps_var(std::move(ps_var)) {}

    bool validate_self() const;

    std::expected<std::string, Unexp_plotDrawer> validateAndDrawPlot() const;

    std::string drawPlot() const;
};

// This is a map of default constructed 'plot_structures' inside an std::variant
// Pass the 'plot_structure' template types that should be used by the library
// This is the only place where one 'selects' these template types


using var_plotTypes =
    std::variant<plot_structures::BarV, plot_structures::BarH, plot_structures::Multiline, plot_structures::Scatter>;

extern template class PlotDrawer<var_plotTypes>;

auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer>;

} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;