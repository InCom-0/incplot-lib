<div align="center">
Incplot is a command like tool for drawing great looking plots in the terminal using unicode characters.

Automatically infers what to display and how based on the data piped in.

Intended for 'as quick and as easy as possible' visualization of small to moderate sized data and information as part of generic worflow.

</div>

## Features ##

* Good looking plots 'out of the box'
* Zero configuration required
* Simple to use, no command line arguments necessary most of the time
* User can optionally specify some or all parameters in any combination
* Accepts piped in data in 'JSON Lines' or 'NDJSON' flat format
* Very fast
* Utilizes full true color to display data groupping where appropriate (eg. Scatter plots)
* Automatic sizing
* Automatic value labels and scaling with metric suffixes
* Automatic tickmarks
* Automatic legend
* Reasonable error messages (especially when the user asks for something that is impossible/unsupported)

## Non-Features ##

* No data wrangling or analysis of any kind
* Not intended for plotting huge datasets
* Doesn't offer the more obscure plot types
* No export of the rendered plot into other formats (such as pictures)
* If the data provided is fundamentally not well suited for plotting (eg. data with no numeric columns, etc.) then it doesn't guarantee good looking plots
* Doesn't show and cannot show 100% visual accuracy due to inherent limitations of terminal interfaces

## Features under consideration ##

* Automatic discovery of terminal colors (usually dubbed 'color theme') ... this would enable matching colors with your existing setup 'out of the box', would be super nice
* Piping data in using other formats

## Disclaimer ##

## General information ##

*

## Technical information ##

## External libraries used ##

* [nlohmann-json](https://github.com/nlohmann/json)
