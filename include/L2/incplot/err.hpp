#pragma once

namespace incom {
namespace terminal_plot {

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
} // namespace terminal_plot
} // namespace incom


