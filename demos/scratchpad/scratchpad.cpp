#include "incplot/desired_plot.hpp"
#include <cstdio>
#include <io.h>
#include <iostream>
#include <print>
#include <stdio.h>
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
    //         std::print("{}", "STD INPUT is not 'pipe' ... exiting");
    //         return 1;
    //         break;
    // }
    if (isatty(fileno(stdin))) {
        std::print("{}", "STD INPUT is not 'pipe' ... exiting");
        return 1;
    }

    std::string input((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

    auto ds = incplot::Parser::parse_NDJSON_intoDS(input);

    auto dp_autoGuessed =
        incplot::DesiredPlot(incplot::DesiredPlot::DP_CtorStruct{.tar_width = 48}).guess_missingParams(ds);

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


    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    auto width  = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    auto height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    std::print("{}\n", width);
    std::print("{}\n", height);


    return 0;
}