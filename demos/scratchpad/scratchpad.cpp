#include <iostream>
#include <optional>
#include <print>
#include <string>
#include <windows.h>



#include <incplot.hpp>


int main() {
    using json = nlohmann::json;

    HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    DWORD  type = GetFileType(hIn);
    switch (type) {
        case FILE_TYPE_PIPE: break;
        default:
            std::print("{}", "STD INPUT is not 'pipe' ... exiting");
            return 1;
            break;
    }

    std::string input((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

    auto ds = incplot::Parser::parse_NDJSON_intoDS(input);


    auto dp_autoGuessed = incplot::DesiredPlot(36).guess_missingParams(ds);

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
    

    return 0;
}