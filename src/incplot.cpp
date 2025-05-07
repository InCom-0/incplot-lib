#include <incplot.hpp>
#include <incplot/detail.hpp>
#include <plot_structures_impl.hpp>



namespace incom {
namespace terminal_plot {

template <typename PS_VAR>
bool PlotDrawer<PS_VAR>::validate_self() const {
    auto validate = [&](auto &&var) -> bool { return var.validate_self(); };
    return std::visit(validate, m_ps_var);
}

template <typename PS_VAR>
std::expected<std::string, Unexp_plotDrawer> PlotDrawer<PS_VAR>::validateAndDrawPlot() const {
    // TODO: Add some validation before drawing
    if (validate_self() == false) { return std::unexpected(Unexp_plotDrawer::plotStructureInvalid); }
    else { return drawPlot(); }
}
template <typename PS_VAR>
std::string PlotDrawer<PS_VAR>::drawPlot() const {
    auto ol = [&](auto &&var) -> std::string { return var.build_plotAsString(); };
    return std::visit(ol, m_ps_var);
}

template class PlotDrawer<var_plotTypes>;


auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds)
    -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer> {

    auto varCpy = var_plotTypes(plot_structures::Scatter{});

    auto ol = [&](auto &&var) -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer> {
        return std::move(var).build_self(dp, ds).and_then(
            [](auto &&ps) -> std::expected<PlotDrawer<var_plotTypes>, Unexp_plotDrawer> {
                return PlotDrawer<var_plotTypes>(ps);
            });
    };
    return std::visit(ol, varCpy);
}

} // namespace terminal_plot
} // namespace incom