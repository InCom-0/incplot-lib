#pragma once

namespace incom {
namespace terminal_plot {

// UNEXPECTED AND OTHER SIMILAR ENUMS
enum class Unexp_plotSpecs {
    TNCII_colByNameNotExist,
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
    plotStructureInvalid,
    barVplot_tooWide,
    catColAndMultipleYValCols,
    areaWidth_insufficient,
    areaHeight_insufficient,
    unknownEerror
};
// UNEXPECTED AND OTHER SIMILAR ENUMS --- END
} // namespace terminal_plot
} // namespace incom