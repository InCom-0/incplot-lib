#pragma once

namespace incom {
namespace terminal_plot {

// UNEXPECTED AND OTHER SIMILAR ENUMS
enum class Unexp_plotSpecs {
    plotType,
    labelCol,
    valCols,
    namesIntoIDs_label,
    namesIntoIDs_vals,
    guessValCols,
    tarWidth,
    tarHeight,
    axisTicks,
    catCol,
    selectedUnuseableCols,
    TScol,
};
enum class Unexp_plotDrawer {
    plotStructureInvalid,
    barVplot_tooWide,
    catColAndMultipleYValCols,
    unknownEerror
};
// UNEXPECTED AND OTHER SIMILAR ENUMS --- END
} // namespace terminal_plot
} // namespace incom