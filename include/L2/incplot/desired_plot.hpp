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

    std::array<Color_CVTS, 12> color_basePalette = {
        Config::color_Vals1_enum, Config::color_Vals2_enum, Config::color_Vals3_enum,
        Config::color_Vals4_enum, Config::color_Vals5_enum, Config::color_Vals6_enum,
    };
    std::array<Color_CVTS, 12> color_bckgrndPalette = {
        Config::color_Bckgrnd1_enum, Config::color_Bckgrnd2_enum, Config::color_Bckgrnd3_enum,
        Config::color_Bckgrnd4_enum, Config::color_Bckgrnd5_enum, Config::color_Bckgrnd6_enum,
    };

    std::optional<bool> valAxesNames_bool  = std::nullopt;
    std::optional<bool> valAxesLabels_bool = std::nullopt;
    std::optional<bool> valAutoFormat_bool = std::nullopt;
    std::optional<bool> legend_bool        = std::nullopt;

    std::optional<double> filter_outsideStdDev  = std::nullopt;
    std::optional<bool>   display_filtered_bool = std::nullopt;

    struct DP_CtorStruct {
        std::optional<size_t>          tar_width      = std::nullopt;
        std::optional<size_t>          tar_height     = std::nullopt;
        std::optional<std::type_index> plot_type_name = std::nullopt;
        std::optional<size_t>          lts_colID      = std::nullopt;
        std::vector<size_t>            v_colIDs       = {};
        std::optional<size_t>          c_colID        = std::nullopt;
        std::array<Color_CVTS, 12>      colors         = {
            Config::color_Vals1_enum, Config::color_Vals2_enum, Config::color_Vals3_enum,
            Config::color_Vals4_enum, Config::color_Vals5_enum, Config::color_Vals6_enum,
        };
        std::array<Color_CVTS, 12> color_bckgrnd = {
            Config::color_Bckgrnd1_enum, Config::color_Bckgrnd2_enum, Config::color_Bckgrnd3_enum,
            Config::color_Bckgrnd4_enum, Config::color_Bckgrnd5_enum, Config::color_Bckgrnd6_enum,
        };
        std::optional<std::string> lts_colName           = std::nullopt;
        std::vector<std::string>   v_colNames            = {};
        std::optional<std::string> c_colName             = std::nullopt;
        std::optional<size_t>      availableWidth        = std::nullopt;
        std::optional<size_t>      availableHeight       = std::nullopt;
        std::optional<double>      filter_outsideStdDev  = std::nullopt;
        std::optional<bool>        display_filtered_bool = Config::display_filtered_bool_default;
    };

    DesiredPlot(DP_CtorStruct &&dp_struct)
        : plot_type_name(std::move(dp_struct.plot_type_name)), cat_colID(std::move(dp_struct.c_colID)),
          cat_colName(std::move(dp_struct.c_colName)), labelTS_colID(std::move(dp_struct.lts_colID)),
          labelTS_colName(std::move(dp_struct.lts_colName)), values_colIDs(std::move(dp_struct.v_colIDs)),
          values_colNames(std::move(dp_struct.v_colNames)), targetWidth(std::move(dp_struct.tar_width)),
          targetHeight(std::move(dp_struct.tar_height)), availableWidth(std::move(dp_struct.availableWidth)),
          availableHeight(std::move(dp_struct.availableHeight)), color_basePalette(std::move(dp_struct.colors)),
          color_bckgrndPalette(std::move(dp_struct.color_bckgrnd)),
          filter_outsideStdDev(std::move(dp_struct.filter_outsideStdDev)),
          display_filtered_bool(std::move(dp_struct.display_filtered_bool)) {}
    DesiredPlot(DP_CtorStruct const &dp_struct)
        : plot_type_name(dp_struct.plot_type_name), cat_colID(dp_struct.c_colID), cat_colName(dp_struct.c_colName),
          labelTS_colID(dp_struct.lts_colID), labelTS_colName(dp_struct.lts_colName), values_colIDs(dp_struct.v_colIDs),
          values_colNames(dp_struct.v_colNames), targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          availableWidth(dp_struct.availableWidth), availableHeight(dp_struct.availableHeight),
          color_basePalette(dp_struct.colors), color_bckgrndPalette(dp_struct.color_bckgrnd),
          filter_outsideStdDev(dp_struct.filter_outsideStdDev), display_filtered_bool(dp_struct.display_filtered_bool) {
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
