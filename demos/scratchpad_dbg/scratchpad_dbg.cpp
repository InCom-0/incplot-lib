#include "incplot/plot_structures.hpp"
#include <incplot.hpp>
#include <incstd.hpp>
#include <typeindex>


#if defined(_WIN64)
#include <windows.h>
#elif defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#endif

void set_cocp() {
#if defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
#elif defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#endif
}

#define DATAFOLDER "../../demos/data/"

int main(int argc, char *argv[]) {

    using namespace std::literals;
    using namespace incom::terminal_plot;

    set_cocp();

    auto dpCtor = incplot::CL_Args::get_dpCtorStruct().front();
    dpCtor.tar_width = 250uz;
    dpCtor.tar_height = 15uz;
    dpCtor.plot_type_name       = std::type_index(typeid(plot_structures::BarHS));
    // dpCtor.tar_width = 100;
    // dpCtor.availableWidth  = 200;
    // dpCtor.availableHeight = 15;
    // dpCtor.filter_outsideStdDev = 0;
    // dpCtor.lts_colID            = 4;
    // dpCtor.v_colIDs             = {0,1,3};

    // auto ds = incplot::parsers::Parser::parse(std::string_view(wineJSON));
    // auto ds_t5 = incplot::Parser::parse(std::string_view(irisJSON_t5));

    std::string_view elcars{DATAFOLDER "elcars_data.csv"sv};
    std::string_view flights{DATAFOLDER "flights_data.ndjson"sv};
    std::string_view iris{DATAFOLDER "iris_data.ndjson"sv};
    std::string_view iris_small{DATAFOLDER "iris_data_small.ndjson"sv};
    std::string_view nile{DATAFOLDER "nile_data.csv"sv};
    std::string_view wine{DATAFOLDER "wine_quality_data.ndjson"sv};

    auto data = incstd::filesys::get_file_textual(iris_small);
    if (not data.has_value()) { std::exit(1); }

    auto ds = parsers::Parser::parse(data.value());
    if (not ds.has_value()) { std::exit(1); }

    auto dp = DesiredPlot(dpCtor);


    auto test_possibilities = plot_structures::evaluate_PSs_asPossibilities<plot_structures::BarV>(dp, ds.value());


    // std::cout << incplot::make_plot_collapseUnExp(dpCtor, data.value()) << '\n';


    return 0;
}