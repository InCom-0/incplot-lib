#include <optional>
#include <print>
#include <string>
#include <windows.h>


#include <incplot.hpp>


int main() {

    std::string testInput(R"({"name":"BioWare","size":750}
{"name":"CD Projekt Red","size":980}
{"name":"Cline","size":6500}
{"name":"DSP_Star_Sector_Resources_61571387-64.csv","size":7558}
{"name":"Dyson Sphere Program","size":4096}
{"name":"Filmy","size":4500}
{"name":"Hudba","size":8500}
{"name":"My Games","size":1500}
{"name":"Obrázky","size":1800}
{"name":"OpenIV","size":2600}
{"name":"PowerToys","size":1000}
{"name":"Project CARS 2","size":4096}
{"name":"Respawn","size":300}
{"name":"Rockstar Games","size":50}
{"name":"The Riftbreaker","size":4096}
{"name":"VTune","size":0})");


    std::string testInput_petal(R"({"sepal_length":5.1,"petal_length":1.4}
{"sepal_length":4.9,"petal_length":1.4}
{"sepal_length":4.7,"petal_length":1.3}
{"sepal_length":4.6,"petal_length":1.5}
{"sepal_length":5,"petal_length":1.4}
{"sepal_length":5.4,"petal_length":1.7}
{"sepal_length":4.6,"petal_length":1.4}
{"sepal_length":5,"petal_length":1.5}
{"sepal_length":4.4,"petal_length":1.4}
{"sepal_length":4.9,"petal_length":1.5}
{"sepal_length":5.4,"petal_length":1.5}
{"sepal_length":4.8,"petal_length":1.6}
{"sepal_length":4.8,"petal_length":1.4}
{"sepal_length":4.3,"petal_length":1.1}
{"sepal_length":5.8,"petal_length":1.2}
{"sepal_length":5.7,"petal_length":1.5}
{"sepal_length":5.4,"petal_length":1.3}
{"sepal_length":5.1,"petal_length":1.4}
{"sepal_length":5.7,"petal_length":1.7}
{"sepal_length":5.1,"petal_length":1.5}
{"sepal_length":5.4,"petal_length":1.7}
{"sepal_length":5.1,"petal_length":1.5}
{"sepal_length":4.6,"petal_length":1}
{"sepal_length":5.1,"petal_length":1.7}
{"sepal_length":4.8,"petal_length":1.9}
{"sepal_length":5,"petal_length":1.6}
{"sepal_length":5,"petal_length":1.6}
{"sepal_length":5.2,"petal_length":1.5}
{"sepal_length":5.2,"petal_length":1.4}
{"sepal_length":4.7,"petal_length":1.6}
{"sepal_length":4.8,"petal_length":1.6}
{"sepal_length":5.4,"petal_length":1.5}
{"sepal_length":5.2,"petal_length":1.5}
{"sepal_length":5.5,"petal_length":1.4}
{"sepal_length":4.9,"petal_length":1.5}
{"sepal_length":5,"petal_length":1.2}
{"sepal_length":5.5,"petal_length":1.3}
{"sepal_length":4.9,"petal_length":1.5}
{"sepal_length":4.4,"petal_length":1.3}
{"sepal_length":5.1,"petal_length":1.5}
{"sepal_length":5,"petal_length":1.3}
{"sepal_length":4.5,"petal_length":1.3}
{"sepal_length":4.4,"petal_length":1.3}
{"sepal_length":5,"petal_length":1.6}
{"sepal_length":5.1,"petal_length":1.9}
{"sepal_length":4.8,"petal_length":1.4}
{"sepal_length":5.1,"petal_length":1.6}
{"sepal_length":4.6,"petal_length":1.4}
{"sepal_length":5.3,"petal_length":1.5}
{"sepal_length":5,"petal_length":1.4}
{"sepal_length":7,"petal_length":4.7}
{"sepal_length":6.4,"petal_length":4.5}
{"sepal_length":6.9,"petal_length":4.9}
{"sepal_length":5.5,"petal_length":4}
{"sepal_length":6.5,"petal_length":4.6}
{"sepal_length":5.7,"petal_length":4.5}
{"sepal_length":6.3,"petal_length":4.7}
{"sepal_length":4.9,"petal_length":3.3}
{"sepal_length":6.6,"petal_length":4.6}
{"sepal_length":5.2,"petal_length":3.9}
{"sepal_length":5,"petal_length":3.5}
{"sepal_length":5.9,"petal_length":4.2}
{"sepal_length":6,"petal_length":4}
{"sepal_length":6.1,"petal_length":4.7}
{"sepal_length":5.6,"petal_length":3.6}
{"sepal_length":6.7,"petal_length":4.4}
{"sepal_length":5.6,"petal_length":4.5}
{"sepal_length":5.8,"petal_length":4.1}
{"sepal_length":6.2,"petal_length":4.5}
{"sepal_length":5.6,"petal_length":3.9}
{"sepal_length":5.9,"petal_length":4.8}
{"sepal_length":6.1,"petal_length":4}
{"sepal_length":6.3,"petal_length":4.9}
{"sepal_length":6.1,"petal_length":4.7}
{"sepal_length":6.4,"petal_length":4.3}
{"sepal_length":6.6,"petal_length":4.4}
{"sepal_length":6.8,"petal_length":4.8}
{"sepal_length":6.7,"petal_length":5}
{"sepal_length":6,"petal_length":4.5}
{"sepal_length":5.7,"petal_length":3.5}
{"sepal_length":5.5,"petal_length":3.8}
{"sepal_length":5.5,"petal_length":3.7}
{"sepal_length":5.8,"petal_length":3.9}
{"sepal_length":6,"petal_length":5.1}
{"sepal_length":5.4,"petal_length":4.5}
{"sepal_length":6,"petal_length":4.5}
{"sepal_length":6.7,"petal_length":4.7}
{"sepal_length":6.3,"petal_length":4.4}
{"sepal_length":5.6,"petal_length":4.1}
{"sepal_length":5.5,"petal_length":4}
{"sepal_length":5.5,"petal_length":4.4}
{"sepal_length":6.1,"petal_length":4.6}
{"sepal_length":5.8,"petal_length":4}
{"sepal_length":5,"petal_length":3.3}
{"sepal_length":5.6,"petal_length":4.2}
{"sepal_length":5.7,"petal_length":4.2}
{"sepal_length":5.7,"petal_length":4.2}
{"sepal_length":6.2,"petal_length":4.3}
{"sepal_length":5.1,"petal_length":3}
{"sepal_length":5.7,"petal_length":4.1}
{"sepal_length":6.3,"petal_length":6}
{"sepal_length":5.8,"petal_length":5.1}
{"sepal_length":7.1,"petal_length":5.9}
{"sepal_length":6.3,"petal_length":5.6}
{"sepal_length":6.5,"petal_length":5.8}
{"sepal_length":7.6,"petal_length":6.6}
{"sepal_length":4.9,"petal_length":4.5}
{"sepal_length":7.3,"petal_length":6.3}
{"sepal_length":6.7,"petal_length":5.8}
{"sepal_length":7.2,"petal_length":6.1}
{"sepal_length":6.5,"petal_length":5.1}
{"sepal_length":6.4,"petal_length":5.3}
{"sepal_length":6.8,"petal_length":5.5}
{"sepal_length":5.7,"petal_length":5}
{"sepal_length":5.8,"petal_length":5.1}
{"sepal_length":6.4,"petal_length":5.3}
{"sepal_length":6.5,"petal_length":5.5}
{"sepal_length":7.7,"petal_length":6.7}
{"sepal_length":7.7,"petal_length":6.9}
{"sepal_length":6,"petal_length":5}
{"sepal_length":6.9,"petal_length":5.7}
{"sepal_length":5.6,"petal_length":4.9}
{"sepal_length":7.7,"petal_length":6.7}
{"sepal_length":6.3,"petal_length":4.9}
{"sepal_length":6.7,"petal_length":5.7}
{"sepal_length":7.2,"petal_length":6}
{"sepal_length":6.2,"petal_length":4.8}
{"sepal_length":6.1,"petal_length":4.9}
{"sepal_length":6.4,"petal_length":5.6}
{"sepal_length":7.2,"petal_length":5.8}
{"sepal_length":7.4,"petal_length":6.1}
{"sepal_length":7.9,"petal_length":6.4}
{"sepal_length":6.4,"petal_length":5.6}
{"sepal_length":6.3,"petal_length":5.1}
{"sepal_length":6.1,"petal_length":5.6}
{"sepal_length":7.7,"petal_length":6.1}
{"sepal_length":6.3,"petal_length":5.6}
{"sepal_length":6.4,"petal_length":5.5}
{"sepal_length":6,"petal_length":4.8}
{"sepal_length":6.9,"petal_length":5.4}
{"sepal_length":6.7,"petal_length":5.6}
{"sepal_length":6.9,"petal_length":5.1}
{"sepal_length":5.8,"petal_length":5.1}
{"sepal_length":6.8,"petal_length":5.9}
{"sepal_length":6.7,"petal_length":5.7}
{"sepal_length":6.7,"petal_length":5.2}
{"sepal_length":6.3,"petal_length":5}
{"sepal_length":6.5,"petal_length":5.2}
{"sepal_length":6.2,"petal_length":5.4}
{"sepal_length":5.9,"petal_length":5.1})");

    auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput);


    auto dp_autoGuessed = incplot::DesiredPlot(96).guess_missingParams(ds);

    if (not dp_autoGuessed.has_value()) {
        std::print("{0}{1}", "Autoguessing of 'DesiresPlot' parameters failed \n", "Exiting ...");
        return 1;
    }

    auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);

    auto outExp = plotDrawer2.validateAndDrawPlot();

    if (not outExp.has_value()) {
        std::print("{0}{1}", "Invalid plot structure", "Exiting ...");
        return 1;
    }

    std::print("{}\n", outExp.value());


    std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Rhodo {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));


    auto ds2 = incplot::Parser::parse_NDJSON_intoDS(testInput_petal);

    auto dp2_autoGuessed = incplot::DesiredPlot(32, 16, "Scatter").guess_missingParams(ds2);

    if (not dp2_autoGuessed.has_value()) {
        std::print("{0}{1}", "Autoguessing of 'DesiresPlot_2' parameters failed \n", "Exiting ...");
        return 1;
    }

    auto plotDrawer3 = incplot::make_plotDrawer(dp2_autoGuessed.value(), ds2);

    auto outExp2 = plotDrawer3.validateAndDrawPlot();

    if (not outExp2.has_value()) {
        std::print("{0}{1}", "Invalid plot structure 2", "Exiting ...");
        return 1;
    }

    std::print("{}\n", outExp2.value());

    std::string firstBLock  = "███████████████";
    std::string firstBLock1 = "█████████";
    std::string firstBLock2 = "███████████████████████";

    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock, incplot::Color_CVTS::Foreground_Green));
    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock1, incplot::Color_CVTS::Foreground_Red));
    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock2, incplot::Color_CVTS::Foreground_Blue));
    std::print("{}\n", "" );
    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock, incplot::Color_CVTS::Foreground_Green));
    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock1, incplot::Color_CVTS::Foreground_Red));
    std::print("{}\n", incplot::TermColors::get_coloured(firstBLock2, incplot::Color_CVTS::Foreground_Blue));

    return 0;
}