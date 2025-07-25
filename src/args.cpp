#include <incplot/args.hpp>
#include <incplot/plot_structures.hpp>
#include <optional>
#include <private/detail.hpp>


namespace incom {
namespace terminal_plot {
std::vector<DesiredPlot::DP_CtorStruct> CL_Args::get_dpCtorStruct(argparse::ArgumentParser &inout_ap, int argc,
                                                                  const char *const *argv) {
    try {
        inout_ap.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << inout_ap;
        std::exit(1);
    }
    std::vector<DesiredPlot::DP_CtorStruct> res;

    auto addOne = [&](std::optional<std::string_view> const &&sv_opt) {
        res.push_back(DesiredPlot::DP_CtorStruct());
        if (auto wdt = inout_ap.present<int>("-w")) { res.back().tar_width = wdt.value(); }
        if (auto hgt = inout_ap.present<int>("-t")) { res.back().tar_height = hgt.value(); }
        if (auto stddev = inout_ap.present<int>("-e")) {
            if (stddev != 0) { res.back().filter_outsideStdDev = stddev.value(); }
            else { res.back().filter_outsideStdDev = std::nullopt; }
        }
        else { res.back().filter_outsideStdDev = Config::filter_withinStdDevMultiple_default; }

        res.back().plot_type_name = sv_opt;
        if (auto optVal = inout_ap.present<int>("-x")) { res.back().lts_colID = optVal.value(); }
        if (auto optVal = inout_ap.present<std::vector<int>>("-y")) {
            res.back().v_colIDs = std::vector<size_t>(optVal.value().begin(), optVal.value().end());
        }
        if (auto optVal = inout_ap.present<int>("-c")) { res.back().c_colID = optVal.value(); }
    };

    if (inout_ap.get<bool>("-b")) { addOne(detail::TypeToString<plot_structures::BarV>()); }
    if (inout_ap.get<bool>("-s")) { addOne(detail::TypeToString<plot_structures::Scatter>()); }
    if (inout_ap.get<bool>("-l")) { addOne(detail::TypeToString<plot_structures::Multiline>()); }
    if (res.empty()) { addOne(std::nullopt); }

    return res;
}

std::vector<DesiredPlot::DP_CtorStruct> CL_Args::get_dpCtorStruct() {
    return std::vector<DesiredPlot::DP_CtorStruct>{DesiredPlot::DP_CtorStruct{.plot_type_name = std::nullopt}};
}

void CL_Args::finishAp(argparse::ArgumentParser &out_ap) {
    out_ap.add_description(
        "Draw coloured plots using unicode symbols inside terminal.\n\nAutomatically infers what to display and "
        "how based on the shape of the data piped in.\nPipe in data in JSON, JSON Lines, NDJSON, CSV or TSV formats. "
        "All "
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
    out_ap.add_argument("-c", "--category").help("Specify the column used to group the data").nargs(1).scan<'d', int>();
    out_ap.add_argument("-e", "--filter-extremes")
        .help(std::format("Specify a multiple of standard deviation above and below which data is filtered('0' means "
                          "no filtering) [default = {}]",
                          Config::filter_withinStdDevMultiple_default))
        .nargs(1)
        .scan<'d', int>();


    out_ap.add_group("Size options");
    out_ap.add_argument("-w", "--width").help("Requested width (in characters)").nargs(1).scan<'d', int>();
    out_ap.add_argument("-t", "--height").help("Requested height (in characters)").nargs(1).scan<'d', int>();
}
} // namespace terminal_plot
} // namespace incom