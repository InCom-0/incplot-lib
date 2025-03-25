#include <codecvt>
#include <locale>
#include <print>
#include <string>
#include <windows.h>


#include <incplot.hpp>


int main() {
    using json = nlohmann::json;

    // HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    // DWORD  type = GetFileType(hIn);
    // switch (type) {
    //     case FILE_TYPE_PIPE: break;
    //     default:
    //         std::cout << "STD INPUT is not 'pipe' ... exiting";⠇
    //         return 1;
    //         break;
    // }

    // std::string input((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

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

    auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput);


    auto dp_autoGuessed = incplot::DesiredPlot(96).guess_missingParams(ds);

    if (not dp_autoGuessed.has_value()) {
        std::print("{0}{1}", "Autoguessing of 'DesiresPlot' parameters failed \n", "Exiting ...");
        return 1;
    }

    std::u32string aaar(U"\u25a0");


    auto aa = U'\u25a0';


    size_t sz = aaar.size();


    auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);

    auto outExp = plotDrawer2.validateAndDrawPlot();


    if (not outExp.has_value()) {
        std::print("{0}{1}", "Invalid plot structure", "Exiting ...");
        return 1;
    }

    auto [val, unit] = incplot::detail::rebase_2_SIPrefix(0.001);


    std::print("{}\n", outExp.value());

    std::print("{}\n", incplot::detail::format_toMax5length(0.00015836));

    std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Rhodo {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));


    auto bd_res = incplot::detail::BrailleDrawer::drawPoints(32, 8, {1, 3, 5,2,4,8,10,10,16,11,10,9,9,9,8,4}, {1,1,1,1,2,2,2,2,5,6,7,12,17,15, 15, 27});


    for (auto const &line : bd_res) { std::print("{0}\n", line); }

    return 0;
}