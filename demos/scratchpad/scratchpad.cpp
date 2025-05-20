#include <print>
#include <string>
#include <unistd.h>

#include <incplot.hpp>

// IMPLEMENTED CROSS PLATFORM SPECIFICS JUST FOR THIS DEMO
#if defined(_WIN64)
#include <windows.h>

#elif defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

bool is_inTerminal() {
#if defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    // auto width  = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    // auto height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);

    if (csbi.dwSize.X == 0 || csbi.dwSize.Y == 0) { return false; }
    else { return true; }

#elif defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
    winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_row == 0 || ws.ws_col == 0) { return false; }
        else { return true; }
    }
    else { return false; }
#endif
}


int main(int argc, char *argv[]) {

    // NOT RUNNING IN CONSOLE TERMINAL
    if (not is_inTerminal()) {
        std::print("{}",
                   "Console screen buffer size equals 0.\nPlease run from inside terminal console window ... exiting");
        std::exit(1);
    }

    // STD INPUT IS NOT PIPE
    if (isatty(fileno(stdin))) {
        std::print("{}", "STD INPUT is not 'pipe' ... exiting");
        std::exit(1);
    }

    argparse::ArgumentParser ap("incplot", "1.0", argparse::default_arguments::help);
    incplot::CL_Args::finishAp(ap);

    std::string const input((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

    for (auto const &dpctr : incplot::CL_Args::get_dpCtorStruct(ap, argc, argv)) {
        std::print("{}\n", incplot::make_plot_collapseUnExp(dpctr, input));
    }

    return 0;
}