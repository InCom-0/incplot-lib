#include <format>
#include <functional>
#include <string>
#include <utility>
#include <variant>

#include <incplot.hpp>
#include <private/detail.hpp>
#include <private/plot_structures_impl.hpp>

#include <magic_enum/magic_enum.hpp>

namespace incom {
namespace terminal_plot {

using incerr_c = incerr::incerr_code;
using enum Unexp_plotDrawer;


template <typename PS_VAR>
bool PlotDrawer<PS_VAR>::validate_self() const {
    auto validate = [&](auto &&var) -> bool { return var.validate_self(); };
    return std::visit(validate, m_ps_var);
}

template <typename PS_VAR>
std::expected<std::string, incerr_c> PlotDrawer<PS_VAR>::validateAndDrawPlot() const {
    // TODO: Add some validation before drawing
    if (validate_self() == false) { return std::unexpected(incerr_c::make(V_PD_nonspecificError)); }
    else { return drawPlot(); }
}
template <typename PS_VAR>
std::string PlotDrawer<PS_VAR>::drawPlot() const {
    auto ol = [&](auto &&var) -> std::string { return var.build_plotAsString(); };
    return std::visit(ol, m_ps_var);
}

// Explicit instantiation definition
// https://en.cppreference.com/w/cpp/language/class_template#Class_template_instantiation
template class PlotDrawer<var_plotTypes>;

auto make_plotDrawer(DesiredPlot const &dp, DataStore const &ds) -> std::expected<PlotDrawer<var_plotTypes>, incerr_c> {

    // This is a map of default constructed 'plot_structures' inside an std::variant
    // This strange incantation makes a lambda with 'index_sequence' and immediatelly invokes it to generate the
    // variantTypeMap with the right types inside it based on var_plotTypes defined elsewhere
    static const auto mp_names2Types = std::invoke(
        []<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return detail::generate_variantTypeMap<plot_structures::Base,
                                                   std::variant_alternative_t<ints, var_plotTypes>...>();
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());

    auto varCpy = mp_names2Types.at(dp.plot_type_name.value());

    auto ol = [&](auto &&var) -> std::expected<PlotDrawer<var_plotTypes>, incerr_c> {
        return std::move(var).build_self(dp, ds).transform(
            [](auto &&ps) -> PlotDrawer<var_plotTypes> { return PlotDrawer<var_plotTypes>(ps); });
    };
    return std::visit(ol, varCpy);
}

// MAIN SIMPLIFIED INTERFACE OF THE LIBRARY
std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData) {

    namespace incp_d = incom::terminal_plot::detail;
    using namespace incom::terminal_plot;

    auto ds = parsers::Parser::parse(inputData);
    if (not ds.has_value()) { return std::unexpected(ds.error()); }

    return DesiredPlot(dp_ctrs)
        .guess_missingParams(ds.value())
        .and_then(incp_d::bind_back(make_plotDrawer, ds.value()))
        .and_then([](auto &&pd) { return pd.validateAndDrawPlot(); });
}

std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData) {
    return make_plot(dp_ctrs, inputData);
}

std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData) {
    auto res = make_plot(dp_ctrs, inputData);
    if (res.has_value()) { return res.value(); }
    else {
        return std::format("{}{}\n\n{}", "Error encoutered. Category of the error: "sv, res.error().category().name(),
                           res.error().message());
    }
}
std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData) {
    return make_plot_collapseUnExp(dp_ctrs, inputData);
}


} // namespace terminal_plot
} // namespace incom