#pragma once

#include <incerr.hpp>
#include <string>
#include <string_view>


namespace incom {
namespace terminal_plot {

using namespace std::literals;

enum class Unexp_plotSpecs {
    // No value '0'
    TNCII_colByNameNotExist = 1,
    GPT_zeroUseableValueColumns,
    GPT_xValTypeStringWhileMoreThan1YvalCols,
    GTSC_noTimeSeriesLikeColumnForMultiline,
    GTSC_noUnusedXvalColumnForScatter,
    GTSC_noStringLikeColumnForLabelsForBarV,
    GTSC_unreachableCodeReached,
    GCC_cantSpecifyCategoryForBarV,
    GCC_noSuitableCatColForBarH,
    GCC_specifiedCatColCantBeUsedAsCatCol,
    GCC_cantSelectCatColAndMultipleYCols,
    GCC_categoryColumnIsNotAllowedForMultiline,
    GVC_selectYvalColIsUnuseable,
    GVC_selectedMoreThan1YvalColForBarV,
    GVC_selectedMoreThan1YvalColForBarH,
    GVC_selectedMoreThanMaxNumOfYvalCols,
    GVC_selectedMoreThanAllowedOfYvalColsForMultiline,
    GVC_notEnoughSuitableYvalCols,
    GVC_cantSelectCatColAndMultipleYCols,
    GZS_widthTooSmall,
    GZS_widthTooLarge,
    GZS_heightTooSmall
};

enum class Unexp_plotDrawer {
    // No value '0'
    C_DSC_areaWidth_insufficient = 1,
    C_DSC_areaHeight_insufficient,
    C_DSC_,
    V_PD_nonspecificError,
};

inline const std::string_view incerr_msg_dispatch(Unexp_plotSpecs &&e) {
    switch (e) {
        case Unexp_plotSpecs::TNCII_colByNameNotExist:
            return "One of the columns specified by name does not exist in the data (ie. the name doesnt exist)\n"
                   "This usually happens because of a typo or because the data "
                   "doesnt have the columns the user thinks it has"sv;
        case Unexp_plotSpecs::GPT_zeroUseableValueColumns:
            return "There are zero columns in the data that can be used for values (ie. numeric ones)\n"
                   "This usually happens when the data cannot be parsed into their numeric representation\n"
                   "It could also mean that the only numeric columns are selected for category or are 'index-like' for "
                   "time series"sv;
        case Unexp_plotSpecs::GPT_xValTypeStringWhileMoreThan1YvalCols:
            return "Multiple value columns are not allowed for vertical bar chart\n"
                   "This usually happens when a string-like column is specified for main axis (-x) while also specifying multiple value columns (-y)"sv;
        case Unexp_plotSpecs::GTSC_noTimeSeriesLikeColumnForMultiline:
            return "There is no column that is 'time-like' for time series. Therefore, line chart cannot be "
                   "constructed\n"
                   "This usually happens when the user wants to draw a line chart, but there is no column that can be "
                   "used for the 'time' axis\n"
                   "A possible fix is to add an index column in the data"sv;
        case Unexp_plotSpecs::GTSC_noUnusedXvalColumnForScatter:
            return "Scatter plot cannot be constructed because there is no free value column to use for the x-axis\n"
                   "This happens when the user selected all available columns for values (-y) and there are none left for the x-axis"sv;
        case Unexp_plotSpecs::GTSC_noStringLikeColumnForLabelsForBarV:
            return "There is no string-like column to be used for label in vertical bar chart"sv;
        case Unexp_plotSpecs::GTSC_unreachableCodeReached: return "Hard library error, unreachable code path reached"sv;
        case Unexp_plotSpecs::GCC_cantSpecifyCategoryForBarV:
            return "Category column cannot be specified for (potentially inferred) vertical bar chart."sv;
        case Unexp_plotSpecs::GCC_noSuitableCatColForBarH:
            return "There is no suitable category column and it is required for the selected horizontal bar chart"sv;
        case Unexp_plotSpecs::GCC_specifiedCatColCantBeUsedAsCatCol:
            return "The user specified a category column that cannot be used as category column\n"
                   "This usually happens because the category columm has more than the max number of categories which is 3"sv;
        case Unexp_plotSpecs::GCC_cantSelectCatColAndMultipleYCols:
            return "It is not allowed to specify both a category column and multiple y-axis value columns\n"
                   "Categories (ie. differently coloured series) can either be specified by one y-value column with "
                   "another column used to categorize those values or they can be specified by using multiple y-value "
                   "columns with each representing one category (this is typical for MultiLine plot)\n"
                   "The user is required to select one or the other"sv;
        case Unexp_plotSpecs::GCC_categoryColumnIsNotAllowedForMultiline:
            return "You cannot specify a category column for use in MultiLine plot."
                   "MultiLine plots must display multiple categories by specifying multiple value columns (y-axis), not a specific category column (which is usually used for other types of plots)"sv;
        case Unexp_plotSpecs::GVC_selectYvalColIsUnuseable:
            return "At least one of the value column the user specified (-y) is unuseable"
                   "This usually happens because the column is 'string-like'"sv;


        default: return "Err msg Unknown error"sv;
    }
}
} // namespace terminal_plot
} // namespace incom

INCERR_REGISTER(incom::terminal_plot::Unexp_plotSpecs);
INCERR_REGISTER(incom::terminal_plot::Unexp_plotDrawer);

#undef INCERR_REGISTER

// namespace std
