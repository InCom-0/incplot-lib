#include <algorithm>
#include <format>
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
using evaAllPoss_rt = decltype(incom::terminal_plot::evaluate_allPSpossibilities(std::declval<DesiredPlot const &>(),
                                                                                 std::declval<DataStore const &>()));

std::expected<DesiredPlot, incerr_c> _reduce_possibilitiesToOne(evaAllPoss_rt const &vec_possibilities) {
    auto fv  = std::views::filter(vec_possibilities, [](auto const &item) { return item.second.has_value(); });
    auto res = std::ranges::max_element(
        fv, [](auto &lhs, auto &rhs) { return lhs.second.value().second < rhs.second.value().second; });

    if (res == fv.end()) { return std::unexpected(incerr_c::make(EVAPS_impossibleToDrawAnyPlot)); }
    else { return res->second.value().first; }
}
std::expected<DesiredPlot, incerr_c> _reduce_possibilitiesToOne(evaAllPoss_rt &&vec_possibilities) {
    return _reduce_possibilitiesToOne(vec_possibilities);
}

std::expected<std::string, incerr_c> _make_plot(DesiredPlot &&dp, std::string_view inputData) {
    using namespace incom::terminal_plot;
    auto ds = parsers::Parser::parse(inputData);
    if (not ds.has_value()) { return std::unexpected(ds.error()); }

    // 1) If dp plot_type_name is set then: a) evaluate that one, if not b) evaluate all and then reduce to the most
    // likely
    // 2) Build the right plot_structure inside a variant
    // 3) Generate plotAsString from the plot_structure variant created in step 2
    auto dp_evaluated = dp.plot_type_name.has_value()
                            ? evaluate_onePSpossibility(dp, ds.value())
                            : _reduce_possibilitiesToOne(evaluate_allPSpossibilities(dp, ds.value()));

    return dp_evaluated.and_then(std::bind_back(build_plot_structure, ds.value())).transform([](auto &&ps_var) {
        return std::visit([&](auto &&ps) -> std::string { return ps.build_plotAsString(); }, ps_var);
    });
}
auto _get_vpt_mpNames2Types(DesiredPlot const &dp, DataStore const &ds) {
    return std::invoke(
        [&]<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return detail::VariantTypeMap<plot_structures::Base,
                                          std::variant_alternative_t<ints, var_plotTypes>...>::gen_typeMap(dp, ds);
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());
}
} // namespace detail


// MAIN SIMPLIFIED INTERFACE OF THE LIBRARY

std::expected<std::string, incerr_c> make_plot(DesiredPlot const &dp_ctrs, std::string_view inputData) {
    return detail::_make_plot(DesiredPlot(dp_ctrs), inputData);
}
std::expected<std::string, incerr_c> make_plot(DesiredPlot &&dp_ctrs, std::string_view inputData) {
    return detail::_make_plot(std::forward<decltype(dp_ctrs)>(dp_ctrs), inputData);
}

std::string make_plot_collapseUnExp(DesiredPlot &&dp_ctrs, std::string_view inputData) {
    auto res = make_plot(std::forward<decltype(dp_ctrs)>(dp_ctrs), inputData);
    if (res.has_value()) { return res.value(); }
    else {
        return std::format("{}{}\n\n{}\n{}{}", "Error encoutered. Error category is: "sv, res.error().category().name(),
                           "Likely cause: "sv, res.error().message(),
                           res.error().get_customMessage() == ""
                               ? ""
                               : std::string("\n\nAdditional context:\n").append(res.error().get_customMessage()));
    }
}
std::string make_plot_collapseUnExp(DesiredPlot const &dp_ctrs, std::string_view inputData) {
    return make_plot_collapseUnExp(DesiredPlot(dp_ctrs), inputData);
}


std::expected<var_plotTypes, incerr_c> build_plot_structure(DesiredPlot const &dp, DataStore const &ds) {
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
            return var.build_self().transform([](auto &&ps) -> var_plotTypes { return var_plotTypes(ps); });
        };
        auto varCpy = found->second;
        return std::visit(ol, varCpy);
    }
};

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
std::expected<DesiredPlot, incerr_c> evaluate_onePSpossibility(DesiredPlot &&dp, DataStore const &ds) {
    return evaluate_onePSpossibility(dp, ds);
}

std::vector<std::pair<std::type_index, std::expected<std::pair<DesiredPlot, size_t>, incerr_c>>>
evaluate_allPSpossibilities(DesiredPlot const &dp, DataStore const &ds) {
    return std::invoke(
        [&]<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return plot_structures::detail_ps::evaluate_PSs<std::variant_alternative_t<ints, var_plotTypes>...>(dp, ds);
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());
}
std::vector<std::pair<std::type_index, std::expected<std::pair<DesiredPlot, size_t>, incerr_c>>>
evaluate_allPSpossibilities(DesiredPlot &&dp, DataStore const &ds) {
    return std::invoke(
        [&]<typename T, T... ints>(std::integer_sequence<T, ints...>) {
            return plot_structures::detail_ps::evaluate_PSs<std::variant_alternative_t<ints, var_plotTypes>...>(
                std::forward<decltype(dp)>(dp), ds);
        },
        std::make_index_sequence<std::variant_size_v<var_plotTypes>>());
}


} // namespace terminal_plot
} // namespace incom