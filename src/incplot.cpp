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

namespace incp_d = incom::terminal_plot::detail;
using incerr_c   = incerr::incerr_code;
using enum Unexp_plotDrawer;


// MAIN SIMPLIFIED INTERFACE OF THE LIBRARY

std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData) {

    using namespace incom::terminal_plot;
    auto ds = parsers::Parser::parse(inputData);
    if (not ds.has_value()) { return std::unexpected(ds.error()); }

    // 1) Guess missing plot parameters
    // 2) Build the right plot_structure inside a variant
    // 3) Generate plotAsString from the plot_structure variant created in step 2
    return DesiredPlot(dp_ctrs)
        .guess_missingParams(ds.value())
        .and_then(std::bind_back(build_plot_structure, ds.value()))
        .transform([](auto &&ps_var) {
            return std::visit([&](auto &&ps) -> std::string { return ps.build_plotAsString(); }, ps_var);
        });
}

std::expected<std::string, incerr_c> make_plot(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData) {
    return make_plot(dp_ctrs, inputData);
}

std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &dp_ctrs, std::string_view inputData) {
    auto res = make_plot(dp_ctrs, inputData);
    if (res.has_value()) { return res.value(); }
    else {
        return std::format("{}{}\n\n{}\n{}{}", "Error encoutered. Error category is: "sv, res.error().category().name(),
                           "Likely cause: "sv, res.error().message(),
                           res.error().get_customMessage() == ""
                               ? ""
                               : std::string("\n\nAdditional context:\n").append(res.error().get_customMessage()));
    }
}
std::string make_plot_collapseUnExp(DesiredPlot::DP_CtorStruct const &&dp_ctrs, std::string_view inputData) {
    return make_plot_collapseUnExp(dp_ctrs, inputData);
}

std::expected<var_plotTypes, incerr_c> build_plot_structure(DesiredPlot const &dp, DataStore const &ds) {
    // This is a map of default constructed 'plot_structures' inside an std::variant
    // This strange incantation create a lambda with 'index_sequence' and immediatelly invokes it to generate the
    // variantTypeMap with the right types inside it based on var_plotTypes defined elsewhere
    static const auto mp_names2Types = std::invoke(
        []<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return detail::generate_variantTypeMap<plot_structures::Base,
                                                   std::variant_alternative_t<ints, var_plotTypes>...>();
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());

    auto varCpy = mp_names2Types.at(dp.plot_type_name.value());

    auto ol = [&](auto &&var) -> std::expected<var_plotTypes, incerr_c> {
        return std::move(var).build_self(dp, ds).transform(
            [](auto &&ps) -> var_plotTypes { return var_plotTypes(ps); });
    };
    return std::visit(ol, varCpy);
};


} // namespace terminal_plot
} // namespace incom