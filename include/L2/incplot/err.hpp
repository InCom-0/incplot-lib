#pragma once

#include <system_error>
#include <utility>

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


class plotSpecs_category_impl : public std::error_category {
public:
    virtual const char *name() const noexcept override { return "plotSpecs Error"; }
    virtual std::string message(int ev) const override {
        switch (ev) {
            case std::to_underlying(Unexp_plotSpecs::TNCII_colByNameNotExist): return "cbnne";
            default:                                                           return "Unknown error";
        }
    }

    static const std::error_category &getSingleton() {
        static const plotSpecs_category_impl instance;
        return instance;
    }
};

class plotDrawer_category_impl : public std::error_category {
public:
    virtual const char *name() const noexcept override { return "plotDrawer Error"; }
    virtual std::string message(int ev) const override {
        switch (ev) {
            case std::to_underlying(Unexp_plotDrawer::C_DSC_areaHeight_insufficient): return "height";
            default:                                                                  return "Unknown error";
        }
    }

    static const std::error_category &getSingleton() {
        static const plotDrawer_category_impl instance;
        return instance;
    }
};

inline const std::error_code make_error_code(Unexp_plotSpecs e) {
    return std::error_code(std::to_underlying(e), plotSpecs_category_impl::getSingleton());
}
inline const std::error_code make_error_condition(Unexp_plotSpecs e) {
    return std::error_code(std::to_underlying(e), plotSpecs_category_impl::getSingleton());
}


} // namespace terminal_plot
} // namespace incom

namespace std {
template <>
struct is_error_code_enum<incom::terminal_plot::Unexp_plotSpecs> : public true_type {};

template <>
struct is_error_code_enum<incom::terminal_plot::Unexp_plotDrawer> : public true_type {};
} // namespace std
