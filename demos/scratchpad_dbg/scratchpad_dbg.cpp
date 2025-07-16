#include <print>
#include <string_view>

#include <data_get.hpp>
#include <incplot.hpp>


#define DATAFOLDER "../../demos/data/"

int main(int argc, char *argv[]) {

    using namespace std::literals;

    auto dpCtor_Structs                         = incplot::CL_Args::get_dpCtorStruct();
    // dpCtor_Structs.front().tar_width = 120uz;
    // dpCtor_Structs.front().plot_type_name = "Multiline";
    // dpCtor_Structs.front().tar_width = 100;
    // dpCtor_Structs.front().availableWidth  = 200;
    // dpCtor_Structs.front().availableHeight = 15;
    dpCtor_Structs.front().filter_outsideStdDev = 6.0;

    // auto ds = incplot::parsers::Parser::parse(std::string_view(wineJSON));
    // auto ds_t5 = incplot::Parser::parse(std::string_view(irisJSON_t5));
    
    std::string_view elcars{DATAFOLDER "elcars_data.csv"sv};
    std::string_view flights{DATAFOLDER "flights_data.ndjson"sv};
    std::string_view iris{DATAFOLDER "iris_data.ndjson"sv};
    std::string_view nile{DATAFOLDER "nile_data.csv"sv};
    std::string_view wine{DATAFOLDER "wine_quality_data.ndjson"sv};

    auto             data = incplot::dataget::get_data(flights);

    if (not data.has_value()) { std::exit(1); }

    for (auto const &dpctr : dpCtor_Structs) {
        std::print("{}\n", incplot::make_plot_collapseUnExp(dpctr, data.value()));
    }

    return 0;
}