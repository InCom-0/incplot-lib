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

    auto dpCtor_Structs                         = incplot::CL_Args::get_dpCtorStruct();
    dpCtor_Structs.front().tar_width = 250uz;
    dpCtor_Structs.front().tar_height = 15uz;
    dpCtor_Structs.front().plot_type_name       = std::type_index(typeid(plot_structures::BarHS));
    // dpCtor_Structs.front().tar_width = 100;
    // dpCtor_Structs.front().availableWidth  = 200;
    // dpCtor_Structs.front().availableHeight = 15;
    // dpCtor_Structs.front().filter_outsideStdDev = 0;
    // dpCtor_Structs.front().lts_colID            = 4;
    // dpCtor_Structs.front().v_colIDs             = {0,1,3};

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

    for (auto const &dpctr : dpCtor_Structs) {
        std::cout << incplot::make_plot_collapseUnExp(dpctr, data.value()) << '\n';
    }

    return 0;
}