#include <iostream>
#include <print>
#include <string>
#include <variant>
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

    std::string testInput(R"({"name":"CMakeFiles","size":0}
{"name":"cmake_install.cmake","size":100}
{"name":"incplot_scratchpad.exe","size":300})");

    auto ttt = incplot::mp_names2Types;
    auto ds  = incplot::Parser::parse_NDJSON_intoDS(testInput);

    auto dp = incplot::DesiredPlot();

    auto dp_autoGuessed = dp.make_autoGuessedDP(ds);
    if (not dp_autoGuessed.has_value()) {
        std::cout << "Autoguessing DesiresPlot failed ... exiting";
        return 1;
    }

    std::string aaar("\u25a0");


    auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds, 64, 32);

    auto outExp = std::visit([&](auto const &pdVariant) { return pdVariant.validateAndDrawPlot(); }, plotDrawer2);


    if (not outExp.has_value()) {
        std::cout << "Invalid plot structure ... exiting";
        return 1;
    }
    std::print("{}", outExp.value());

    return 0;
}