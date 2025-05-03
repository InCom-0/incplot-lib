incplot-lib is a library implementing the core features necessary to make [inplot](https://github.com/InCom-0/incplot) work.

Naturally though, it can be used for other things as well

## Features ##

* DataStore - A way to store the data to be plotted in a sensible manner
* DesiredPlot - A way to specify what is to be plotted and how and all the necessary details. Also provides implementation of (sort of smart) autoguessing of parameters that user hasn't specified
* plot_structures namespace - Type-based ways to 'draw' (that is render into a unicode string) various kinds of plots
* PlotDrawer - Encapsulation of the above functionality into directly usable thing

## Disclaimer ##

## General information ##

*

## Technical information ##

## External libraries used ##

* [nlohmann-json](https://github.com/nlohmann/json)
* [csv2](https://github.com/p-ranav/csv2)
* [argparse](https://github.com/p-ranav/argparse)
* [utf-cpp](https://github.com/ww898/utf-cpp)
