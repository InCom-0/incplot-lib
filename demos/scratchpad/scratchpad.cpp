#define OOF_IMPL
#include <print>
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

    std::string testInput(R"({"name":"BioWare","size":0}
{"name":"CD Projekt Red","size":0}
{"name":"Cline","size":0}
{"name":"DSP_Star_Sector_Resources_61571387-64.csv","size":12558}
{"name":"Dyson Sphere Program","size":4096}
{"name":"Filmy","size":0}
{"name":"Hudba","size":0}
{"name":"My Games","size":0}
{"name":"Obrázky","size":0}
{"name":"OpenIV","size":0}
{"name":"PowerToys","size":0}
{"name":"Project CARS 2","size":4096}
{"name":"Respawn","size":0}
{"name":"Rockstar Games","size":0}
{"name":"The Riftbreaker","size":4096}
{"name":"VTune","size":0})");

    auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput);


    auto dp_autoGuessed = incplot::DesiredPlot(64).guess_missingParams(ds);

    if (not dp_autoGuessed.has_value()) {
        std::print("{0}{1}", "Autoguessing of 'DesiresPlot' parameters failed \n", "Exiting ...");
        return 1;
    }

    std::string aaar("\u25a0");


    auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);

    auto outExp = plotDrawer2.validateAndDrawPlot();


    if (not outExp.has_value()) {
        std::print("{0}{1}", "Invalid plot structure", "Exiting ...");
        return 1;
    }

    auto [val, unit] = incplot::detail::rebase_2_SIPrefix(0.001);


    std::print("{}\n", outExp.value());

    std::print("{}\n", incplot::detail::format_toMax4length(0.00015836));

    std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Reduta {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));

    return 0;
}