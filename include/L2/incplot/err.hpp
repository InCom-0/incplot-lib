#pragma once


#include <incerr.hpp>
#include <incplot/config.hpp>


namespace incom {
namespace terminal_plot {

using namespace std::literals;

enum class Unexp_plotSpecs {
    // No value '0'
    TNCII_colByNameNotExist = 1,
    GPT_explicitlySpecifiedDifferentPlotType,
    GPT_zeroUseableValueColumns,
    GPT_xValTypeStringWhileMoreThan1YvalCols,
    GTSC_noTimeSeriesLikeColumnForMultiline,
    GTSC_cantSpecifyTScolForOtherThanMultiline,
    GTSC_noUnusedXvalColumnForScatter,
    GTSC_noStringLikeColumnForLabelsForBarPlot,
    GTSC_unreachableCodeReached,
    GCC_cantSpecifyCategoryForOtherThanScatter,
    GCC_noSuitableCatColForBarVM,
    GCC_specifiedCatColCantBeUsedAsCatCol,
    GCC_cantSelectCatColAndMultipleYCols,
    GCC_categoryColumnIsNotAllowedForMultiline,
    GVC_selectYvalColIsUnuseable,
    GVC_selectedMoreThan1YvalColForBarV,
    GVC_selectedMoreThan6YvalColForBarXM,
    GVC_selectedMoreThanMaxNumOfYvalCols,
    GVC_selectedMoreThanAllowedOfYvalColsForMultiline,
    GVC_notEnoughSuitableYvalCols,
    GVC_cantSelectCatColAndMultipleYCols,
    GZS_widthTooSmall,
    GZS_widthTooLarge,
    GSZ_tarWidthLargerThanAvailableWidth,
    GSZ_iferredTargetWidthLargerThanAvailableWidth,
    GSZ_iferredTargetWidthLargerThanDefaultWidth,
    GZS_heightTooSmall,
    TEST_t1
};


enum class Unexp_plotDrawer {
    // No value '0'
    INI_labelTS_colID_isNull = 1,
    INI_values_colIDs_isEmpty,
    INI_values_rowCount_isZero,
    C_DSC_areaWidth_insufficient,
    C_DSC_areaHeight_insufficient,
    C_DSC_,
    V_PD_nonspecificError,
};
enum class Unexp_parser {
    // No value '0'
    JSON_malformattedArrayLike = 1,
    JSON_objectsNotOfSameSize,
    JSON_valueTypeDoesntMatch,
    JSON_keyNameDoesntMatch,
    JSON_isEmpty,
    JSON_topLevelEleNotArrayOrObject,
    JSON_parserBackendError,
    JSON_unhandledCellType,
    NDJSON_braceCountDoesntMatch,
    NDJSON_braceCountDoesntMatchNLcount,
    NDJSON_isEmpty,
    NDJSON_isNotFlat,
    CSV_containsZeroNewLineChars,
    CSV_headerHasMoreItemsThanDataRow,
    CSV_headerHasLessItemsThanDataRow,
    CSV_valueTypeDoesntMatch,
    CSV_parserBackendError,
    CSV_unhandledCellType,
};

std::string_view incerr_msg_dispatch(Unexp_plotSpecs &&e);
// std::string_view incerr_msg_dispatch(Unexp_plotDrawer &&e);


} // namespace terminal_plot
} // namespace incom

INCERR_REGISTER(incom::terminal_plot::Unexp_plotSpecs, incom::terminal_plot);
INCERR_REGISTER(incom::terminal_plot::Unexp_plotDrawer, incom::terminal_plot);
INCERR_REGISTER(incom::terminal_plot::Unexp_parser, incom::terminal_plot);

#undef INCERR_REGISTER

// namespace std
