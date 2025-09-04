
#include <format>
#include <string_view>

#include <incplot/err.hpp>


namespace incom {
namespace terminal_plot {

using namespace std::literals;

std::string_view incerr_msg_dispatch(Unexp_plotSpecs &&e) {
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
        case Unexp_plotSpecs::GTSC_noStringLikeColumnForLabelsForBarPlot:
            return "There is no string-like column to be used for label in vertical bar chart"sv;
        case Unexp_plotSpecs::GTSC_selectedTScolNotFoundInData:
            return "The column the user selected for primary axis (-x) was not found in the data.\n"
                   "The most likely reason is that the column ID provided is higher than the highest column ID"sv;
        case Unexp_plotSpecs::GTSC_selectedTScolIsNotTimeSeriesLike:
            return "The column the user selected as primary axis (-x) for (multi)line plot is not 'time "
                   "series like'.\n"
                   "Line plot requires the primary axis column to contain arithmetic values with regular interval "
                   "between them\n"
                   "A possible workaround is to include an index column in the data"sv;
        case Unexp_plotSpecs::GTSC_cantSelectTSColToBeTheSameAsCatCol:
            return "Category column (-c or --category) and primary axis column (-x) need to be different columns"sv;
            ;
        case Unexp_plotSpecs::GTSC_cantSelectTSColToBeOneOfTheValCols:
            return "One of the columns the user selected as values columm (-y) is also selected as the primary axis "
                   "column (-x).\n"
                   "The type of column overlap is not allowed."sv;
        case Unexp_plotSpecs::GTSC_unreachableCodeReached: return "Hard library error, unreachable code path reached"sv;
        case Unexp_plotSpecs::GCC_cantSpecifyCategoryForOtherThanScatter:
            return "Category column cannot be specified for any plot type other then Scatter"sv;
        case Unexp_plotSpecs::GCC_noSuitableCatColForScatter:
            return "There is no suitable category column and it is required for the selected scatter plot"sv;
        case Unexp_plotSpecs::GCC_specifiedCatColCantBeUsedAsCatCol:
            {
                static std::string GCC_cccbuacc_r;
                GCC_cccbuacc_r =
                    std::format("The user specified a category column that cannot be used as category column\n"
                                "This usually happens because the category columm has more than the max number of "
                                "categories which is: {}",
                                Config::max_maxNumOfCategories);
                return std::string_view(GCC_cccbuacc_r);
            }

        case Unexp_plotSpecs::GCC_cantSelectCatColAndMultipleYCols:
            return "It is not allowed to specify both a category column and multiple y-axis value columns\n"
                   "Categories (ie. differently coloured series) can either be specified by one y-value column with "
                   "another column used to categorize those values or they can be specified by using multiple y-value "
                   "columns with each representing one category (this is typical for MultiLine plot)\n"
                   "The user is required to select one or the other"sv;
        case Unexp_plotSpecs::GCC_categoryColumnIsNotAllowedForMultiline:
            return "It is not possible to specify a category column for use in MultiLine plot\n"
                   "MultiLine plots must display multiple categories by specifying multiple value columns (y-axis), not a specific category column (which is usually used for other types of plots)"sv;
        case Unexp_plotSpecs::GVC_selectYvalColIsUnuseable:
            return "At least one of the value column the user specified (-y) is unuseable"
                   "This usually happens because the column is 'string-like'"sv;
        case Unexp_plotSpecs::GVC_selectedMoreThan1YvalColForBarV:
            return "It is not allowed to select more than one value column (-y) for use in (potentially inferred) vertical bar plot"sv;
        case Unexp_plotSpecs::GVC_selectedMoreThan1YvalColForScatterCat:
            return "It is not allowed to select more than one value columns (-y) for use in (potentially inferred) scatter plot together with category column (-c)"sv;
        case Unexp_plotSpecs::GVC_selectedMoreThan3YvalColForScatterNonCat:
            return "It is not allowed to select more than three value columns (-y) for use in (potentially inferred) scatter plot without category column"sv;
        case Unexp_plotSpecs::GVC_selectedMoreThan6YvalColForBarXM:
            return "It is not allowed to select more than six value columns (-y) for use in (potentially inferred) multibar plot"sv;
        case Unexp_plotSpecs::GVC_selectedMoreThanMaxNumOfYvalCols:
            {
                static const std::string GVC_mnyvc_r{
                    std::format("The user selected more than a maximum number of value columns (-y) which is: {}",
                                Config::max_numOfValColsScatterCat)};
                return std::string_view(GVC_mnyvc_r);
            }

        case Unexp_plotSpecs::GVC_selectedMoreThanAllowedOfYvalColsForMultiline:
            {
                static std::string GVC_anc_r{
                    std::format("The user selected more than a maximum number of value columns for MultiLine "
                                "plot (-y) which is: {}",
                                Config::max_maxNumOfCategories)};
                return std::string_view(GVC_anc_r);
            }

        case Unexp_plotSpecs::GVC_notEnoughSuitableYvalCols:
            return "There is not enough suitable value columns (y-axis) in the data to satisfy the minimum number required for some plot\n"sv;
        case Unexp_plotSpecs::GVC_cantSelectCatColAndMultipleYCols:
            return "It is not allowed to specify both a category column and multiple y-axis value columns\n"
                   "Categories (ie. differently coloured series) can either be specified by one y-value column with "
                   "another column used to categorize those values or they can be specified by using multiple y-value "
                   "columns with each representing one category (this is typical for MultiLine plot)\n"
                   "The user is required to select one or the other"sv;
        case Unexp_plotSpecs::GZS_widthTooSmall:
            {
                static std::string GZS_wts_r{
                    std::format("The user cannot specify a width of less than: {}", Config::min_plotWidth)};
                return std::string_view(GZS_wts_r);
            }
        case Unexp_plotSpecs::GZS_widthTooLarge:
            {
                static std::string GZS_wtl_r{
                    std::format("The user cannot specify a width of more than: {}", Config::max_plotWidth)};
                return std::string_view(GZS_wtl_r);
            }
        case Unexp_plotSpecs::GSZ_tarWidthLargerThanAvailableWidth:
            return "Specified target width (-w) must not be larger than available width.\n"
                   "Note: available width is inferred from the size of the terminal window automatically"sv;
        case Unexp_plotSpecs::GSZ_iferredTargetWidthLargerThanAvailableWidth:
            return "Inferred target width must not be larger than available width.\n"
                   "You may have selected a plot type which when used with the data supplied would be wider than "
                   "terminal window.\n"
                   "Note: available width is inferred from the size of the terminal window automatically"sv;
        case Unexp_plotSpecs::GSZ_iferredTargetWidthLargerThanDefaultWidth:
            return "Inferred target width must not be larger than default width if available width cannot be "
                   "determined.\n"
                   "Note: incplot was unable to obtain available width from the terminal, generally this should never happen and indicates something else being very wrong."sv;
        case Unexp_plotSpecs::GZS_heightTooSmall:
            {
                static std::string GZS_hts_r{
                    std::format("The user cannot specify a height of less than: {}", Config::max_plotWidth)};
                return std::string_view(GZS_hts_r);
            }
        case Unexp_plotSpecs::CAPF_unhandledError: return "Undocumented error type"sv;
        default:                                   return "Undocumented error type"sv;
    }
}

// COMMENTED OUT UNTILL THIS ERROR TYPE IS DOCUMENTED
// TODO: Implement the method below
// std::string_view incerr_msg_dispatch(Unexp_plotDrawer &&e) {
//     switch (e) {
//         default: return "Undocumented error type"sv;
//     }
// }

} // namespace terminal_plot
} // namespace incom
