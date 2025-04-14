#include "incplot/config.hpp"
#include <io.h>
#include <iostream>
#include <print>
#include <string>
#include <windows.h>


#include <argparse/argparse.hpp>
#include <incplot.hpp>


int main(int argc, char *argv[]) {
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
        incplot::DesiredPlot(incplot::DesiredPlot::DP_CtorStruct{.tar_width = 100}).guess_missingParams(ds);

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


    argparse::ArgumentParser ap("incplot");
    // auto                    &ap_plotTypes = ap.add_mutually_exclusive_group();

    ap.add_description("Draw coloured plots with unicode inside terminal.\n\nAutomatically infers what to display and "
                       "how.\nAll arguments are optional but can be used when one wants something 'unusual'");


    ap.add_group("Plot type options");
    ap.add_argument("-b", "--barV").help("Draw vertical bar plot").flag().nargs(0);
    ap.add_argument("-s", "--scatter").help("Draw scatter plot").flag().nargs(0);
    ap.add_argument("-l", "--line").help("Draw (multi)line plot").flag().nargs(0);


    ap.add_group("Values options");
    ap.add_argument("-x", "--main").help("Specify primary axis by column ID").nargs(1).scan<'i', int>();
    ap.add_argument("-y", "--values").help("Specify secondary axis values by column IDs").nargs(1,6).scan<'i', int>();


    ap.add_group("Size options");
    ap.add_argument("-w", "--width")
        .help("Requested width (in characters)")
        .nargs(1)
        .default_value("0")
        .implicit_value(std::to_string(width + incom::terminal_plot::Config::delta_toInferredWidth))
        .scan<'i', int>();


    ap.add_argument("-t", "--height")
        .help("Requested height (in characters)")
        .nargs(1)
        .default_value("0")
        .implicit_value(std::to_string(height + incom::terminal_plot::Config::delta_toInferredHeight))
        .scan<'i', int>();


    ap.add_epilog("Pipe in data in JSON Lines (or NDJSON) format");


    try {
        ap.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << ap;
        return 1;
    }


    return 0;
}