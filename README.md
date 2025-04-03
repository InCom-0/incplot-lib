<div align="center">
Incplot is a command like tool for drawing great looking plots in the terminal using unicode characters.

Automatically infers what to display and how based on the data piped in.

Intended for 'as quick and as easy as possible' visualization of small to moderate sized data and information as part of generic worflow.

</div>

## Features ##

* Good looking plots 'out of the box'
* Super simple to use, no command line arguments necessary most of the time
* User can optionally specify some or all parameters
* Very fast
* Can utilize colors to display data groupping
* Can mix full true color where appropriate (eg. Scatter plots)
* Automatic sizing
* Automatic value labels and scaling with metric prefixes
* Automatic tickmarks
* Automatic legend
* Reasonable error message (especially if the user asks for something that is impossible/unsupported)

## Non-Features ##

* No data wrangling or analysis of any kind
* Not intended for plotting huge datasets
* Doesn't offer the more obscure plot types
* No export
* Cannot create good looking plots if the data provided is fundamentally not well suited for plotting (eg. data with no numeric columns)
* Due to limitations of the terminal interface the plots don't and can't show 100% visual accuracy
* Generally not intended for 'data professionals'

## Disclaimer ##

## General information ##

*

## Technical information ##

## External libraries used ##

* [nlohmann-json](https://github.com/nlohmann/json)
