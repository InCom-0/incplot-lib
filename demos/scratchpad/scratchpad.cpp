#include <codecvt>
#include <locale>
#include <string>
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

    std::print("{}\n", incplot::detail::format_toMax4length(0.00015836));

    std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Rhodo {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue),
               incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::string                                                 converted = conv.to_bytes(aa);

    std::u32string tempStr(1, aa);
    std::string    converted2(tempStr.begin(), tempStr.end());


    std::array<std::array<char32_t, 2>, 4> ttt{U'⠁', U'⠈', U'⠂', U'⠐', U'⠄', U'⠠', U'⡀', U'⢀'};

    char32_t test = ttt[0][0] | ttt[0][1] | ttt[1][0] | ttt[1][1] | ttt[2][0] | ttt[2][1] | ttt[3][0]| ttt[3][0];

    std::u32string testStr(1, test);
    std::string    retu(testStr.begin(), testStr.end());

    std::print("{}", conv.to_bytes(testStr));

    return 0;
}