#pragma once

#include <expected>
#include <optional>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <incplot/err.hpp>
#include <typeindex>
#include <utility>


namespace incom {
namespace terminal_plot {

// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'
class DesiredPlot {
private:
    struct ColumnParams {
        size_t categoryCount;
        double standDev;
        double mean;

        bool is_categoriesSameSize;
        bool is_categoryLike;
        bool is_sameRepeatingSubsequences;
        bool is_sameRepeatingSubsequences_whole;
        bool is_timeSeriesLikeIndex;
        bool is_allValuesNonNegative;
        bool is_allValuesIdentical;
    };


public:
    std::vector<unsigned int> filterFlags      = {};
    std::vector<ColumnParams> m_colAssessments = {};


    // BUILDING METHODS
    static std::expected<DesiredPlot, incerr::incerr_code> compute_colAssessments(DesiredPlot    &&dp,
                                                                                  DataStore const &ds);

    static std::expected<DesiredPlot, incerr::incerr_code> transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                      DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> guess_sizes(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> guess_plotType(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> guess_TSCol(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> guess_catCol(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> guess_valueCols(DesiredPlot &&dp, DataStore const &ds);

    static std::expected<DesiredPlot, incerr::incerr_code> guess_TFfeatures(DesiredPlot &&dp, DataStore const &ds);

    static void compute_filterFlags_r_void(DesiredPlot &dp, DataStore const &ds);
    static std::expected<DesiredPlot, incerr::incerr_code> compute_filterFlags(DesiredPlot &&dp, DataStore const &ds);


    std::optional<std::type_index> plot_type_name = std::nullopt;

    // Category column ID
    std::optional<size_t>      cat_colID   = std::nullopt; // ID in m_data
    std::optional<std::string> cat_colName = std::nullopt;

    // Label for use in timeseries
    std::optional<size_t>      labelTS_colID   = std::nullopt; // ID in m_data
    std::optional<std::string> labelTS_colName = std::nullopt;

    std::vector<size_t>      values_colIDs   = {}; // IDs in m_data
    std::vector<std::string> values_colNames = {};

    std::optional<size_t> targetWidth  = std::nullopt;
    std::optional<size_t> targetHeight = std::nullopt;

    std::optional<size_t> availableWidth  = std::nullopt;
    std::optional<size_t> availableHeight = std::nullopt;

    incom::standard::console::color_schemes::scheme16 colScheme = incstd::console::color_schemes::defaultScheme16;
    std::array<size_t, 12uz>                          colOrder;
    std::vector<std::string>                          colScheme_fg_rawANSI;
    std::vector<std::string>                          colScheme_bg_rawANSI;
    std::string                                       colScheme_fg_default;
    std::string                                       colScheme_bg_default;
    std::string                                       colScheme_brightBlack;

    std::array<ANSI::SGR_map, 12> color_basePalette    = Config::paletteSGR_native_fg;
    std::array<ANSI::SGR_map, 12> color_bckgrndPalette = Config::paletteSGR_native_bg;

    std::optional<bool> valAxesNames_bool  = std::nullopt;
    std::optional<bool> valAxesLabels_bool = std::nullopt;
    std::optional<bool> valAutoFormat_bool = std::nullopt;
    std::optional<bool> legend_bool        = std::nullopt;

    std::optional<double> filter_outsideStdDev  = std::nullopt;
    std::optional<bool>   display_filtered_bool = std::nullopt;
    std::optional<bool>   htmlMode_bool         = std::nullopt;
    std::optional<bool>   forceRGB_bool         = std::nullopt;

    std::vector<std::string> additionalInfo = {};

    struct DP_CtorStruct {
        std::optional<size_t>          tar_width      = std::nullopt;
        std::optional<size_t>          tar_height     = std::nullopt;
        std::optional<std::type_index> plot_type_name = std::nullopt;
        std::optional<size_t>          lts_colID      = std::nullopt;
        std::vector<size_t>            v_colIDs       = {};
        std::optional<size_t>          c_colID        = std::nullopt;

        incom::standard::console::color_schemes::scheme16 colScheme = incstd::console::color_schemes::defaultScheme16;
        std::array<size_t, 12uz>                          colOrder  = Config::colOrderDefault;
        std::array<ANSI::SGR_map, 12>                     colors    = Config::paletteSGR_native_fg;
        std::array<ANSI::SGR_map, 12>                     color_bckgrnd = Config::paletteSGR_native_bg;

        std::optional<std::string> lts_colName           = std::nullopt;
        std::vector<std::string>   v_colNames            = {};
        std::optional<std::string> c_colName             = std::nullopt;
        std::optional<size_t>      availableWidth        = std::nullopt;
        std::optional<size_t>      availableHeight       = std::nullopt;
        std::optional<double>      filter_outsideStdDev  = std::nullopt;
        std::optional<bool>        display_filtered_bool = Config::display_filtered_bool_default;
        std::optional<bool>        htmlMode_bool         = Config::htmlMode_bool_default;
        std::optional<bool>        forceRGB_bool         = Config::forceRGB_bool_default;

        std::vector<std::string> additionalInfo = {};
    };

private:
    void ctor_finisher(DP_CtorStruct const &dp_struct) {
        // Both htmlMode and forceRGB both mean that we will be using SGR [38;2;r;g;bm] for all coloring
        if ((htmlMode_bool.has_value() && htmlMode_bool.value()) ||
            (forceRGB_bool.has_value() && forceRGB_bool.value())) {
            for (auto const &id : dp_struct.colOrder) {
                colScheme_fg_rawANSI.push_back(ANSI::get_fg(dp_struct.colScheme.palette.at(id)));
                colScheme_bg_rawANSI.push_back(ANSI::get_bg(dp_struct.colScheme.palette.at(id)));
            }
            colScheme_fg_default  = ANSI::get_fg(dp_struct.colScheme.foreground);
            colScheme_bg_default  = ANSI::get_bg(dp_struct.colScheme.backgrond);
            colScheme_brightBlack = ANSI::get_fg(dp_struct.colScheme.palette.at(8));
        }
        // Otherwise we will use terminal native palette (but not necessarily actually knowing what specific colors they
        // are) This uses the SGR [30-37m] ... but only for plots where we don't 'blend' colors
        else {
            for (auto const &id : dp_struct.colOrder) {
                colScheme_fg_rawANSI.push_back(std::string(
                    ANSI::get_fromSGR_direct(ANSI::ANSI_col16_to_SGRmap_fg(static_cast<ANSI_Color16>(id)))));
                colScheme_bg_rawANSI.push_back(std::string(
                    ANSI::get_fromSGR_direct(ANSI::ANSI_col16_to_SGRmap_bg(static_cast<ANSI_Color16>(id)))));
            }

            colScheme_fg_default = std::string(ANSI::get_fromSGR_direct(ANSI::SGR_map::FG_Default));
            colScheme_bg_default = std::string(ANSI::get_fromSGR_direct(ANSI::SGR_map::BG_Default));
            colScheme_brightBlack =
                std::string(ANSI::get_fromSGR_direct(ANSI::ANSI_col16_to_SGRmap_fg(ANSI::ANSI_Color16::Bright_Black)));
        }
    }

public:
    DesiredPlot(DP_CtorStruct &&dp_struct)
        : plot_type_name(std::move(dp_struct.plot_type_name)), cat_colID(std::move(dp_struct.c_colID)),
          cat_colName(std::move(dp_struct.c_colName)), labelTS_colID(std::move(dp_struct.lts_colID)),
          labelTS_colName(std::move(dp_struct.lts_colName)), values_colIDs(std::move(dp_struct.v_colIDs)),
          values_colNames(std::move(dp_struct.v_colNames)), targetWidth(std::move(dp_struct.tar_width)),
          targetHeight(std::move(dp_struct.tar_height)), availableWidth(std::move(dp_struct.availableWidth)),
          availableHeight(std::move(dp_struct.availableHeight)), colScheme(std::move(dp_struct.colScheme)),
          colOrder(std::move(dp_struct.colOrder)), colScheme_fg_rawANSI{}, colScheme_bg_rawANSI{},

          color_basePalette(std::move(dp_struct.colors)), color_bckgrndPalette(std::move(dp_struct.color_bckgrnd)),
          filter_outsideStdDev(std::move(dp_struct.filter_outsideStdDev)),
          display_filtered_bool(std::move(dp_struct.display_filtered_bool)),
          htmlMode_bool(std::move(dp_struct.htmlMode_bool)), forceRGB_bool(std::move(dp_struct.forceRGB_bool)),
          additionalInfo(std::move(dp_struct.additionalInfo)) {
        ctor_finisher(dp_struct);
    }
    DesiredPlot(DP_CtorStruct const &dp_struct)
        : plot_type_name(dp_struct.plot_type_name), cat_colID(dp_struct.c_colID), cat_colName(dp_struct.c_colName),
          labelTS_colID(dp_struct.lts_colID), labelTS_colName(dp_struct.lts_colName), values_colIDs(dp_struct.v_colIDs),
          values_colNames(dp_struct.v_colNames), targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          availableWidth(dp_struct.availableWidth), availableHeight(dp_struct.availableHeight),
          colScheme(dp_struct.colScheme), colOrder(dp_struct.colOrder), colScheme_fg_rawANSI{}, colScheme_bg_rawANSI{},
          colScheme_fg_default(ANSI::get_fg(dp_struct.colScheme.foreground)),
          colScheme_bg_default(ANSI::get_bg(dp_struct.colScheme.backgrond)),
          colScheme_brightBlack(ANSI::get_bg(dp_struct.colScheme.palette.at(8))), color_basePalette(dp_struct.colors),
          color_bckgrndPalette(dp_struct.color_bckgrnd), filter_outsideStdDev(dp_struct.filter_outsideStdDev),
          display_filtered_bool(dp_struct.display_filtered_bool), htmlMode_bool(dp_struct.htmlMode_bool),
          forceRGB_bool(dp_struct.forceRGB_bool), additionalInfo(dp_struct.additionalInfo) {
        ctor_finisher(dp_struct);
    }

    // Create a new copy and guess_missingParams on it.
    std::expected<DesiredPlot, incerr::incerr_code> build_guessedParamsCPY(this DesiredPlot &self,
                                                                           DataStore const  &ds) {
        return DesiredPlot(self).guess_missingParams(ds);
    }

    // Guesses the missing 'desired parameters' and returns a DesiredPlot with those filled in
    // Variation on a 'builder pattern'
    // Normally called 'in place' on 'DesiredPlot' instance constructed as rvalue
    // If impossible to guess or otherwise the user desires something impossible returns Err_plotSpecs.
    std::expected<DesiredPlot, incerr::incerr_code> guess_missingParams(this DesiredPlot &&self, DataStore const &ds);

    template <typename... PSs>
    std::expected<DesiredPlot, incerr::incerr_code> guess_mostLikely();
};


} // namespace terminal_plot
} // namespace incom
