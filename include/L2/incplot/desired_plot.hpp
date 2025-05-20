#pragma once

#include <expected>
#include <optional>

#include <incplot/config.hpp>
#include <incplot/datastore.hpp>
#include <incplot/misc.hpp>


namespace incom {
namespace terminal_plot {

// Encapsulates the 'instructions' information about the kind of plot that is desired by the user
// Big feature is that it includes logic for 'auto guessing' the 'instructions' that were not provided explicitly
// Basically 4 important things: 1) Type of plot, 2) Labels to use (if any), 3) Values to use, 4) Size in 'chars'
class DesiredPlot {
private:
    struct ColumnParams {
        size_t categoryCount;

        bool is_categoriesSameSize;
        bool is_categoryLike;
        bool is_sameRepeatingSubsequences;
        bool is_sameRepeatingSubsequences_whole;
        bool is_timeSeriesLikeIndex;
    };

    std::vector<ColumnParams> m_colAssessments = {};

    // BUILDING METHODS
    static std::expected<DesiredPlot, Unexp_plotSpecs> compute_colAssessments(DesiredPlot &&dp, DataStore const &ds);

    static std::expected<DesiredPlot, Unexp_plotSpecs> transform_namedColsIntoIDs(DesiredPlot    &&dp,
                                                                                  DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_plotType(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_TSCol(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_catCol(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_valueCols(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_sizes(DesiredPlot &&dp, DataStore const &ds);
    static std::expected<DesiredPlot, Unexp_plotSpecs> guess_TFfeatures(DesiredPlot &&dp, DataStore const &ds);


public:
    std::optional<std::string> plot_type_name;

    // Category column ID
    std::optional<size_t>      cat_colID; // ID in colTypes
    std::optional<std::string> cat_colName;

    // Label for use in timeseries
    std::optional<size_t>      labelTS_colID; // ID in colTypes
    std::optional<std::string> labelTS_colName;

    // TODO: Make both 'values_' into std::optional as well to keep the logic the same for all here
    std::vector<size_t>      values_colIDs; // IDs in colTypes
    std::vector<std::string> values_colNames;

    std::optional<size_t> targetHeight;
    std::optional<size_t> targetWidth;

    std::array<Color_CVTS, 6> color_basePalette;

    std::optional<bool> valAxesNames_bool;
    std::optional<bool> valAxesLabels_bool;
    std::optional<bool> valAutoFormat_bool;
    std::optional<bool> legend_bool;

    // TODO: Provide some compile time programmatic way to set the default sizes here
    struct DP_CtorStruct {
        std::optional<size_t>      tar_width      = std::nullopt;
        std::optional<size_t>      tar_height     = std::nullopt;
        std::optional<std::string> plot_type_name = std::nullopt;
        std::optional<size_t>      lts_colID      = std::nullopt;
        std::vector<size_t>        v_colIDs       = {};
        std::optional<size_t>      c_colID        = std::nullopt;
        std::array<Color_CVTS, 6>  colors         = {
            Config::color_Vals1_enum, Config::color_Vals2_enum, Config::color_Vals3_enum,
            Config::color_Vals4_enum, Config::color_Vals5_enum, Config::color_Vals6_enum,
        };
        std::optional<std::string> lts_colName = std::nullopt;
        std::vector<std::string>   v_colNames  = {};
        std::optional<std::string> c_colName   = std::nullopt;
    };

    DesiredPlot(DP_CtorStruct &&dp_struct)
        : targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          plot_type_name(std::move(dp_struct.plot_type_name)), labelTS_colID(std::move(dp_struct.lts_colID)),
          values_colIDs(std::move(dp_struct.v_colIDs)), cat_colID(std::move(dp_struct.c_colID)),
          color_basePalette(std::move(dp_struct.colors)), labelTS_colName(std::move(dp_struct.lts_colName)),
          values_colNames(std::move(dp_struct.v_colNames)), cat_colName(std::move(dp_struct.c_colName)) {}
    DesiredPlot(DP_CtorStruct const &dp_struct)
        : targetWidth(dp_struct.tar_width), targetHeight(dp_struct.tar_height),
          plot_type_name(dp_struct.plot_type_name), labelTS_colID(dp_struct.lts_colID),
          values_colIDs(dp_struct.v_colIDs), cat_colID(dp_struct.c_colID), color_basePalette(dp_struct.colors),
          labelTS_colName(dp_struct.lts_colName), values_colNames(dp_struct.v_colNames),
          cat_colName(dp_struct.c_colName) {}

    // Create a new copy and guess_missingParams on it.
    std::expected<DesiredPlot, Unexp_plotSpecs> build_guessedParamsCPY(this DesiredPlot &self, DataStore const &ds) {
        return DesiredPlot(self).guess_missingParams(ds);
    }

    // Guesses the missing 'desired parameters' and returns a DesiredPlot with those filled in
    // Variation on a 'builder pattern'
    // Normally called 'in place' on 'DesiredPlot' instance constructed as rvalue
    // If impossible to guess or otherwise the user desires something impossible returns Err_plotSpecs.
    std::expected<DesiredPlot, Unexp_plotSpecs> guess_missingParams(this DesiredPlot &&self, DataStore const &ds);
};


} // namespace terminal_plot
} // namespace incom
