incplot-lib is a library implementing the core features necessary to make [inplot](https://github.com/InCom-0/incplot) work.

Naturally though, it can potentially be used for other things as well

## Main logical structure ##

* Parser - Parse a text-based input (JSON, JSON Lines, NDJSON, CSV, TSV) into DataStore type
* DataStore - A way to store the data to be plotted in a sensible manner
* DesiredPlot - A way to specify what is to be plotted and how and all the necessary details. Also provides implementation of (sort of) smart autoguessing of parameters that the user hasn't specified
* plot_structures namespace - Type-based ways to 'draw' (that is render into a unicode string) various kinds of plots

## General information ##

## Technical information ##

* Uses bleeding edge (as of May 2025) features from C++23 language as well as from C++23 STL extensively
* Natively cross-platform without any platform specific code behind macros (in the library itself)
* Compiles with GCC 14.2+ and Clang 20.1.3+ on Linux, MacOS
* Compiles with MSVC 19.44+, GCC 14.2+ (through MSYS2) and Clang 20.1.3+ (through MSYS2) on Windows
* Uses flexible design patterns combining both classic OOP and functional programming features of C++23 (ie. monadic operations)
* Combines classic inheritance, 'builder pattern', 'deducing this' from C++23 and std::expected and its monadic operations from C++23 into one design pattern that is easy to change/refactor/add to, that is way less error prone, has great error handling and stellar performance characteristics through compile-time polymorphism ... mostly seen in 'plot_structures_impl.hpp'
* Generally doesn't use pointer semantics anywhere (unmanaged or managed)
* To do what it does incplot-lib requires a number of dependencies.
    * One bigger group is related to being able to manipulate/modify/work with fonts (enables html output) 
    * Secondly there are dependencies requires for parsing of input data
    * Thirdly, there are generic utility libraries

## External libraries used ##
* [otfccxx](https://github.com/InCom-0/otfccxx)
    * [harfbuzz](https://github.com/harfbuzz/harfbuzz)
    * [otfcc_cmake](https://github.com/InCom-0/otfcc_cmake)
    * [fmem](https://github.com/InCom-0/fmem)
    * [woff2](https://github.com/InCom-0/woff2)
    * [base64tl](https://github.com/InCom-0/base64)
    
* [nlohmann-json](https://github.com/nlohmann/json)
* [csv2](https://github.com/p-ranav/csv2)
* [argparse](https://github.com/p-ranav/argparse)
* [utf-cpp](https://github.com/ww898/utf-cpp)
* [magic_enum](https://github.com/Neargye/magic_enum)
* [incstd](https://github.com/InCom-0/incstd)
    * [xxHash](https://github.com/Cyan4973/xxHash)
    * [unordered_dense](https://github.com/martinus/unordered_dense)
* [incerr](https://github.com/InCom-0/incerr)
