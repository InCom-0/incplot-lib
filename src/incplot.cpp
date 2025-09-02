#include <algorithm>
#include <functional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <variant>

#include <incplot.hpp>
#include <magic_enum/magic_enum.hpp>
#include <private/detail.hpp>
#include <private/plot_structures_impl.hpp>


namespace incom {
namespace terminal_plot {

using incerr_c = incerr::incerr_code;
using enum Unexp_plotDrawer;


namespace detail {
std::expected<std::string, incerr_c> _make_plot(DesiredPlot &&dp, std::string_view inputData) {
    using namespace incom::terminal_plot;
    auto ds = parsers::Parser::parse(inputData);
    if (not ds.has_value()) { return std::unexpected(ds.error()); }

    using evaAllPoss_rt = decltype(incom::terminal_plot::evaluate_allPSpossibilities(
        std::declval<DesiredPlot const &>(), std::declval<DataStore const &>()));

    auto lam_reducePossibilitiesToOne = [](evaAllPoss_rt &&vec_possibilities) -> std::expected<DesiredPlot, incerr_c> {
        auto fv  = std::views::filter(vec_possibilities, [](auto const &item) { return item.second.has_value(); });
        auto res = std::ranges::max_element(
            fv, [](auto const &lhs, auto const &rhs) { return lhs.second.value().second < rhs.second.value().second; });

        if (res == fv.end()) { return std::unexpected(incerr_c::make(EVAPS_impossibleToDrawAnyPlot)); }
        else { return res->second.value().first; }
    };

    auto lam_get_bpas = [](auto &&ps_var) {
        return std::visit([&](auto &&ps) -> std::string { return ps.build_plotAsString(); }, ps_var);
    };

    // 1) If dp plot_type_name is set then: a) evaluate that one, if not b) evaluate all and then reduce to the most
    // likely
    // 2) Build the right plot_structure inside a variant
    // 3) Generate plotAsString from the plot_structure variant created in step 2
    if (dp.plot_type_name.has_value()) {
        return evaluate_onePSpossibility(dp, ds.value())
            .and_then(std::bind_back(build_plotStructure, ds.value()))
            .transform(lam_get_bpas);
    }
    else {
        return lam_reducePossibilitiesToOne(evaluate_allPSpossibilities(dp, ds.value()))
            .and_then(std::bind_back(build_plotStructure, ds.value()))
            .transform(lam_get_bpas);
    }
    std::unreachable();
}
auto _get_vpt_mpNames2Types(DesiredPlot const &dp, DataStore const &ds) {
    return std::invoke(
        [&]<size_t... IDXs>(std::index_sequence<IDXs...>) {
            return detail::VariantTypeMap<plot_structures::Base,
                                          std::variant_alternative_t<IDXs, var_plotTypes>...>::gen_typeMap(dp, ds);
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());
}
} // namespace detail


// IMLEMENTATION OF MAIN SIMPLIFIED INTERFACE OF THE LIBRARY
// Making the plot
// IE: 1) evaluate possibilities, 2) build plotStructure, 3) render into a final string (or render error message)
std::expected<std::string, incerr_c> make_plot(DesiredPlot &&dp_ctrs, std::string_view inputData) {
    return detail::_make_plot(std::forward<decltype(dp_ctrs)>(dp_ctrs), inputData);
}

std::string make_plot_collapseUnExp(DesiredPlot &&dp_ctrs, std::string_view inputData) {
    auto res = make_plot(std::forward<decltype(dp_ctrs)>(dp_ctrs), inputData);
    if (res.has_value()) { return res.value(); }

    const auto &error  = res.error();
    std::string result = std::string("Error encountered. Error category is: ")
                             .append(error.category().name())
                             .append("\n\nLikely cause: ")
                             .append(error.message());

    if (not error.get_customMessage().empty()) {
        result.append("\n\nAdditional context:\n");
        result.append(error.get_customMessage());
    }
    return result;
}

// Building plot structure (ie. all the different parts of the plot individually)
std::expected<var_plotTypes, incerr_c> build_plotStructure(DesiredPlot const &dp, DataStore const &ds) {
    if (not dp.plot_type_name.has_value()) { return std::unexpected(incerr_c::make(BPS_dpIsNullopt)); }

    // This is a map of constructed 'plot_structures' inside an std::variant
    // This strange incantation create a lambda with 'index_sequence' and immediatelly invokes it to generate the
    // variantTypeMap with the right types inside it based on var_plotTypes defined elsewhere
    auto const mp_names2Types = detail::_get_vpt_mpNames2Types(dp, ds);
    auto       varCpy         = mp_names2Types.at(dp.plot_type_name.value());

    // Check if what is specified in dp 'plot_type_name' is one of the types in var_plotTypes
    if (auto const &found = mp_names2Types.find(dp.plot_type_name.value()); found == mp_names2Types.end()) {
        return std::unexpected(incerr_c::make(BPS_typeIndexInDPisNotIn_var_plotTypes));
    }
    else {
        auto ol = [&](auto &var) -> std::expected<var_plotTypes, incerr_c> {
            return var.build_self().transform([](auto &&ps) -> var_plotTypes { return var_plotTypes(std::move(ps)); });
        };
        auto varCpy = found->second;
        return std::visit(ol, varCpy);
    }
};

// Desired plot possibilities evaluations
std::expected<DesiredPlot, incerr_c> evaluate_onePSpossibility(DesiredPlot const &dp, DataStore const &ds) {
    // Cannot evaluate what isn't specified in dp 'plot_type_name'
    if (not dp.plot_type_name.has_value()) { return std::unexpected(incerr_c::make(EVAPS_dpIsNullopt)); }

    auto const mp_names2Types = detail::_get_vpt_mpNames2Types(dp, ds);

    // Check if what is specified in dp 'plot_type_name' is one of the types in var_plotTypes
    if (auto const &found = mp_names2Types.find(dp.plot_type_name.value()); found == mp_names2Types.end()) {
        return std::unexpected(incerr_c::make(EVAPS_typeIndexInDPisNotIn_var_plotTypes));
    }
    else {
        auto ol = [&](auto &var) -> std::expected<DesiredPlot, incerr_c> {
            return plot_structures::detail_ps::evaluate_PS<std::remove_cvref_t<decltype(var)>>(dp, ds);
        };

        auto varCpy = found->second;
        return std::visit(ol, varCpy);
    }
}

std::vector<std::pair<std::type_index, std::expected<std::pair<DesiredPlot, size_t>, incerr_c>>>
evaluate_allPSpossibilities(DesiredPlot const &dp, DataStore const &ds) {
    return std::invoke(
        [&]<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return plot_structures::detail_ps::evaluate_PSs<std::variant_alternative_t<ints, var_plotTypes>...>(dp, ds);
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());
}

} // namespace terminal_plot
} // namespace incom