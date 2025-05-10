incplot-lib is a library implementing the core features necessary to make [inplot](https://github.com/InCom-0/incplot) work.

Naturally though, it can potentially be used for other things as well

## Main logical structural ##

* Parser - Parses a text-based input (JSON, JSON Lines, NDJSON, CSV, TSV) into DataStore type
* DataStore - A way to store the data to be plotted in a sensible manner
* DesiredPlot - A way to specify what is to be plotted and how and all the necessary details. Also provides implementation of (sort of) smart autoguessing of parameters that the user hasn't specified
* plot_structures namespace - Type-based ways to 'draw' (that is render into a unicode string) various kinds of plots
* PlotDrawer - Encapsulation of the above functionality into directly usable thing

## General information ##

## Technical information ##

* Uses bleeding edge (as of May 2025) features from C++23 language as well as from C++23 standard library extensively
* Nativelly cross-platform without any platform specific code behind macros (in the library itself)
* Compiles with GCC 14.2+ and Clang 20.1.3+ on Windows (through MSYS2), Linux and MacOS
* Doesn't (yet) compile with MSVC (presumably because of some as of yet missing C++23 feature implementation)
* Uses flexible design patterns combining both classic OOP and functional programming features of C++23 (ie. monadic operations)
* Combines class inheritance, 'builder pattern', 'deducing this' from C++23 and std::expected and its monadic operations from C++23 into one design pattern that is easy to change/refactor/add to, that is way less error prone, has great error handling and stellar performance characteristics through compile-time polymorphism ... mostly seen in 'plot_structures_impl.hpp'
* Generally doesn't use pointer semantics anywhere (unmanaged or managed)

## External libraries used ##

* [nlohmann-json](https://github.com/nlohmann/json)
* [csv2](https://github.com/p-ranav/csv2)
* [argparse](https://github.com/p-ranav/argparse)
* [utf-cpp](https://github.com/ww898/utf-cpp)
* [magic_enum](https://github.com/Neargye/magic_enum)
