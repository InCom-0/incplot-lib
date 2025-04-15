#pragma once

#include "desired_plot.hpp"
#include "incplot/plot_structures.hpp"
#include <argparse/argparse.hpp>
#include <expected>
#include <incplot/config.hpp>
#include <incplot/desired_plot.hpp>
#include <optional>
#include <string_view>
#include <vector>


namespace incom {
namespace terminal_plot {

struct CL_Args {

    static constexpr std::vector<DesiredPlot::DP_CtorStruct> get_dpCtorStruct(argparse::ArgumentParser &out_ap,
                                                                              int argc, const char *const *argv) {

        try {
            out_ap.parse_args(argc, argv);
        }
        catch (const std::exception &err) {
            std::cerr << err.what() << std::endl;
            std::cerr << out_ap;
            std::exit(1);
        }
        std::vector<DesiredPlot::DP_CtorStruct> res;

        auto addOne = [&](std::optional<std::string_view> const &&sv_opt) {
            res.push_back(DesiredPlot::DP_CtorStruct());
            if (auto wdt = out_ap.present<int>("-w")) { res.back().tar_width = wdt.value(); }
            if (auto hgt = out_ap.present<int>("-t")) { res.back().tar_height = hgt.value(); }

            res.back().plot_type_name = sv_opt;
            if (auto optVal = out_ap.present<int>("-x")) { res.back().l_colID = optVal.value(); }
            if (auto optVal = out_ap.present<std::vector<int>>("-y")) {
                res.back().v_colIDs = std::vector<size_t>(optVal.value().begin(), optVal.value().end());
            }
            if (auto optVal = out_ap.present<int>("-c")) { res.back().c_colID = optVal.value(); }
        };

        if (out_ap.get<bool>("-b")) { addOne(detail::TypeToString<plot_structures::BarV>()); }
        if (out_ap.get<bool>("-s")) { addOne(detail::TypeToString<plot_structures::Scatter>()); }
        if (out_ap.get<bool>("-l")) { addOne(detail::TypeToString<plot_structures::Multiline>()); }
        if (res.empty()) { addOne(std::nullopt); }

        return res;
    }

    static constexpr void finishAp(argparse::ArgumentParser &out_ap, int const &sb_width, int const &sb_height) {
        out_ap.add_description(
            "Draw coloured plots using unicode symbols inside terminal.\n\nAutomatically infers what to display and "
            "how based on the shape of the data piped in.\nPipe in data in JSON Lines (or NDJSON) format. All "
            "arguments "
            "are optional");

        out_ap.add_group("Plot type options");
        out_ap.add_argument("-b", "--barV").help("Draw vertical bar plot [flag]").flag().nargs(0);
        out_ap.add_argument("-s", "--scatter").help("Draw scatter plot [flag]").flag().nargs(0);
        out_ap.add_argument("-l", "--line").help("Draw (multi)line plot [flag]").flag().nargs(0);


        out_ap.add_group("Values options");
        out_ap.add_argument("-x", "--main").help("Specify primary axis by column ID").nargs(1).scan<'d', int>();
        out_ap.add_argument("-y", "--values")
            .help("Specify secondary axis values by column IDs")
            .nargs(1, 6)
            .scan<'d', int>();
        out_ap.add_argument("-c", "--category")
            .help("Specify the column used to group the data")
            .nargs(1)
            .scan<'d', int>();


        out_ap.add_group("Size options");
        out_ap.add_argument("-w", "--width").help("Requested width (in characters)").nargs(1).scan<'d', int>();
        // .default_value(sb_width + incom::terminal_plot::Config::delta_toInferredWidth);


        out_ap.add_argument("-t", "--height").help("Requested height (in characters)").nargs(1).scan<'d', int>();
        // .default_value(sb_height + incom::terminal_plot::Config::delta_toInferredHeight);
    }
};
} // namespace terminal_plot
} // namespace incom