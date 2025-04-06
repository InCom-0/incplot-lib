<div align="center">
Incplot is a command like tool for drawing great looking plots in the terminal using unicode characters.

Automatically infers what to display and how based on the data piped in.

Intended for 'as quick and as easy as possible' visualization of small to moderate sized data and information as part of generic worflow.

</div>

## Features ##

* Good looking plots 'out of the box'
* Simple to use, no command line arguments necessary most of the time
* User can optionally specify some or all parameters
* Accepts piped in data in 'JSON Lines' or 'NDJSON' flat format
* Very fast
* Utilizes full true colors to display data groupping where appropriate (eg. Scatter plots)
* Automatic sizing
* Automatic value labels and scaling with metric suffixes
* Automatic tickmarks
* Automatic legend
* Reasonable error message (especially when the user asks for something that is impossible/unsupported)

## Non-Features ##

* No data wrangling or analysis of any kind
* Not intended for plotting huge datasets
* Doesn't offer the more obscure plot types
* No export of the rendered plot into other formats (such as pictures)
* If the data provided is fundamentally not well suited for plotting (eg. data with no numeric columns) then it doesn't guarantee good looking plots
* Doesn't show and cannot show 100% visual accuracy due to limitations of the terminal interface

## Disclaimer ##

## General information ##

*

## Technical information ##

## External libraries used ##

* [nlohmann-json](https://github.com/nlohmann/json)
